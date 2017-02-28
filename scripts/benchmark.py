#!/usr/bin/python3
import argparse
import subprocess
import csv
import os
from database import Database
import configs
import reporters as reps


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Run benchmarker and insert results in SQLite file'
        )

    parser.add_argument(
            'benchmarks', nargs='*', default=[],
            help='Configurations to benchmark (ids)'
        )

    parser.add_argument(
            '--index', '-i', metavar='index name',
            help='Include all benchmarks for the given index'
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


def run_make(options, index):
    subprocess.check_call(
            ['cmake'] + ['-D%s=%s' % d for d in options.items()] + ['..']
        )

    subprocess.check_call(
            ['make', 'bench']
        )

    subprocess.check_call(
            ['make', index]
        )


def get_commit():
    return subprocess.check_output(
            ['git', 'rev-parse', 'HEAD']
        ).strip()


def detect_dimension(filename):
    i = len(filename) - 1

    while filename[i].isdigit() and i > 0:
        i -= 1

    return int(filename[i + 1:])


def get_benchmark_ids(db, index):
    benchmarks = db.connection.execute(
            """
            select `benchmark_id`
            from `benchmark` inner join `config` using (`config_id`)
            where `index` = ?
            """,
            [index]
        ).fetchall()

    return set(b['benchmark_id'] for b in benchmarks)


def main():
    args = parse_arguments()
    db = Database(args.database)
    benchmarks = set(args.benchmarks)
    build_dir = args.builddir

    if (args.index):
        benchmarks |= get_benchmark_ids(db, args.index)

    # Prepare for out of source compilation
    subprocess.check_call(['mkdir', '-p', build_dir])
    os.chdir(build_dir)

    for benchmark_id in benchmarks:

        # Gather information
        commit = get_commit()
        benchmark = db.get_by_id('benchmark', benchmark_id)

        if not benchmark:
            print('Error: Benchmark %s does not exist' % benchmark_id)
            continue

        config_id = benchmark['config_id']
        config = configs.get_by_id(db, config_id)
        dataset = benchmark['dataset']
        reporters = reps.get(db, benchmark_id)

        if not reporters:
            print('No reporters to run for benchmark %d' % benchmark_id)
            continue

        dimension = detect_dimension(dataset)

        # Build
        definitions = dict(config['definitions'], D=dimension)
        run_make(definitions, config['index'])

        # Run the bencmark
        results = subprocess.check_output(
                [
                    './bench',
                    config['index'],
                    '../' + dataset,
                ] + ['%(name)s:../%(arguments)s' % r for r in reporters]
            ).decode('utf-8')

        run_id = db.insert(
                'run', benchmark_id=benchmark_id, commit=commit
            ).lastrowid

        # Save results
        for result in zip(results.split('\n\n'), reporters):
            results_reader = csv.DictReader(
                    result[0].split('\n'),
                    delimiter='\t'
                )

            db.insertmany(
                    'result',
                    (dict(r, run_id=run_id, reporter_id=result[1]['id'])
                        for r in results_reader)
                )

        db.commit()


main()
