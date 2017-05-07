#!/usr/bin/python3

"""
Optimizes the runtime of an index given a set of configuration parameters using
hill climbing with random restarts. Does not store results, but prints the
(assumed) optimal parameters at the end.
"""
import argparse
from functools import partial
from random import random, gauss
from math import exp, e
from database import Database
import compile_for
import benchmark
import os
import asyncio


def compose2(f, g):

    def composite(*args, **kwargs):
        return f(g(*args, **kwargs))

    return composite


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Optimize parameters for index runtime'
        )

    parser.add_argument(
            'config', metavar='<base config>',
            help='Config to use for fixed parameters'
        )

    parser.add_argument(
            'params', metavar='<name>:<start value>', nargs='+',
            help='Parameters to optimize and its min and max value'
        )

    parser.add_argument(
            '--benchmarks', '-m', metavar='<benchmarks>',
            nargs='+', help='Benchmarks to optimize for'
        )

    parser.add_argument(
            '--suites', '-s', metavar='<suite>[ <suite>]', nargs='+',
            help='Suite whos benchmarks to optimize for'
        )

    parser.add_argument(
            '--restrictions', '-r', nargs='*', default=[],
            help='Restrictions on the parameters'
        )

    parser.add_argument(
            '--database', '-d', metavar='filename', default='results',
            help='Path to sqlite database file'
        )

    parser.add_argument(
            '--builddir', '-b', metavar='path', default='build',
            help='Path to build dir'
        )

    return parser.parse_args()


def hash_dict(d):
    return hash(','.join(
            '%s=%s' % p for p in sorted(d.items())
        ))


def memoized(func):
    cache = {}

    def wrapper_func(d):
        key = hash_dict(d)

        if key not in cache:
            cache[key] = func(d)
        else:
            print("Fetched from cache: ", d)

        return cache[key]

    return wrapper_func


def clamp(value, minimum, maximum):
    """
    Clamps a value to the given range
    """
    return min(maximum, max(minimum, value))


def prepare_options(parameters):
    """
    Converts a set of parameters used during optimization to index options.
    This allows relative parameters to be used while passing absolute options
    to the index.
    """
    options = dict(parameters)

    # Convert from percentage m to absolute m
    if 'm' in options and 'M' in options:
        M = options['M']
        options['m'] = clamp(round(M * options['m'] / 100), 1, M//2)

    return options


def anneal(start_solution, validator, evaluator):
    """
    Does the actual search
    """
    evaluate = compose2(evaluator, prepare_options)
    current = (evaluate(start_solution), start_solution)
    best = current
    temperature = current[0]
    unsuccessful = 0
    iterations = 0

    while unsuccessful < 20:

        iterations += 1
        print('\n--- Iteration %d (t: %.2f) ---' % (iterations, temperature))

        # Generate candidate solution
        candidate = None
        while candidate is None or not validator(candidate):
            candidate = dict(
                    (k, round(gauss(v, 0.5 * start_solution[k])))
                    for k, v in current[1].items()
                )

        # Evaluate
        score = evaluate(candidate)

        # Move to the candidate?
        delta = score - current[0]
        if delta < 0 or e * random() < exp(-delta / temperature):
            current = (score, candidate)
            print("New solution: ", score, " (params: ", candidate, ")")
            unsuccessful = 0

            # Update the best seen so far
            if delta < 0:
                best = current
            else:
                print('(soluion is worse than previous)')

        else:
            print('Keeping previous solution')
            unsuccessful += 1

        # Decrease temperature
        temperature *= 0.99

    print("Finished in %d iterations" % iterations)
    return best


def check_restrictions(restrictions, point):
    return all(eval(r, dict(point)) for r in restrictions)


def evaluate(db, config, benchmarks, options):

    total_runtime = 0

    for b in benchmarks:
        print(
                'Evaluating ' +
                ' '.join('%s=%s' % i for i in options.items()) +
                '... '
            )

        compile_for.compile(db, config, override_options=options)

        results = asyncio.get_event_loop().run_until_complete(
                benchmark.benchmark(db, config, b, False)
            )

        # Sum up results
        total_runtime += sum(
                min(
                    float(r['value'])
                    for r in reporter_results
                    if r['name'] == 'PAPI_REAL_NSEC'
                ) for (reporter_results, _) in results
            )

    print('Result: %s' % total_runtime)
    return total_runtime


def main():
    args = parse_arguments()
    db = Database(args.database)
    config_id = int(args.config)
    benchmark_ids = [int(v) for v in args.benchmarks]

    os.chdir(args.builddir)

    point = dict((p, int(v)) for (p, v) in (a.split(':') for a in args.params))

    print(
            anneal(
                    point,
                    partial(check_restrictions, args.restrictions),
                    memoized(
                            partial(evaluate, db, config_id, benchmark_ids)
                        )
                )
        )


if __name__ == '__main__':
    main()
