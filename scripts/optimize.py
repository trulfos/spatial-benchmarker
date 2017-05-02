#!/usr/bin/python3

"""
Optimizes the runtime of an index given a set of configuration parameters using
hill climbing with random restarts. Does not store results, but prints the
(assumed) optimal parameters at the end.

TODO: Some parameters are not integers?
"""
import argparse
from functools import partial
from itertools import product
from database import Database
import compile_for
import benchmark
import os
import asyncio


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Optimize parameters for index runtime'
        )

    parser.add_argument(
            'task', metavar='<config>:<benchmark>',
            help='Configuration-benchhmark pairs to optimize for'
        )

    parser.add_argument(
            'params', metavar='<name>:<start value>', nargs='+',
            help='Parameters to optimize and its min and max value'
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


def climb(start_point, validator, evaluator):
    """
    Does the actual search
    """
    current = start_point
    best = float('inf')
    step_size = 5

    while step_size > 1:
        # Generate candidate solutions
        candidates = (
                dict(current, **{p: current[p] + s})
                for (p, s) in product(current.keys(), [step_size, -step_size])
            )

        # Filter out invalid solutions
        candidates = filter(validator, candidates)

        # Evaluate all
        evaluated = [(evaluator(c), c) for c in candidates]

        try:
            local_best = min(evaluated)
        except ValueError:
            local_best = None

        # Update best or decrease step size?
        if local_best is not None and best > local_best[0]:
            (best, current) = local_best
            print("New best: ", best, " (params: ", current, ")")
        else:
            print("Decreasing step size")
            step_size = max(round(step_size / 2), 1)

    return current


def check_restrictions(restrictions, point):
    return all(eval(r, dict(point)) for r in restrictions)


def evaluate(db, config_id, benchmark_id, parameters):
    print('Evaluating for ', parameters)
    compile_for.compile(db, config_id, override_options=parameters)

    results = asyncio.get_event_loop().run_until_complete(
            benchmark.benchmark(db, config_id, benchmark_id, True)
        )

    # Sum up results
    return sum(
            sum(
                float(r['value'])
                for r in reporter_results
                if r['name'] == 'PAPI_REAL_NSEC'
            ) for (reporter_results, _) in results
        )


def main():
    args = parse_arguments()
    db = Database(args.database)
    (config_id, benchmark_id) = map(int, args.task.split(':'))

    os.chdir(args.builddir)

    point = dict((p, int(v)) for (p, v) in (a.split(':') for a in args.params))

    print(
            climb(
                    point,
                    partial(check_restrictions, args.restrictions),
                    partial(evaluate, db, config_id, benchmark_id)
                )
        )


if __name__ == '__main__':
    main()
