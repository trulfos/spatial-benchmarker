#!/usr/bin/python3
import argparse
import subprocess
import csv
import os
from database import Database
import configs


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Run benchmarker and insert results in SQLite file'
        )

    parser.add_argument(
            'benchmark',
            help='Name of benchmark to use'
        )

    parser.add_argument(
            'configs', nargs='+',
            help='Configurations to benchmark (ids)'
        )

    parser.add_argument(
            '--database', '-d', metavar='filename', default='results',
            help='Path to sqlite database file'
        )

    parser.add_argument(
            '--report', '-r', metavar='reporter', default='runtime',
            help='Reporter to use for collecting results'
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
        )


def main():
    args = parse_arguments()
    benchmark = args.benchmark
    db = Database(args.database)

    # Read benchmark dimension
    with open(benchmark + "dimension") as f:
        dimension = f.read().strip()

    # Prepare for out of source compilation
    subprocess.check_call('mkdir -p build'.split())
    os.chdir('build')

    for config_id in set(args.configs):
        # Gather information
        commit = get_commit()
        config = configs.get_by_id(db, config_id)

        if not config:
            print('Error: Config %s does not exist' % config_id)
            continue

        # Build
        definitions = dict(config['definitions'], D=dimension)
        run_make(definitions, config['index'])

        # Run the bencmark
        results = subprocess.check_output([
                    './bench',
                    config['index'],
                    '../' + benchmark,
                    '-r', args.report
            ]).decode('utf-8')

        # Save results
        results_reader = csv.DictReader(results.split('\n'), delimiter='\t')
        run_id = db.insert(
                'run', config_id=config_id, commit=commit, benchmark=benchmark
            ).lastrowid

        db.insertmany(
                'result',
                (dict(r, run_id=run_id) for r in results_reader)
            )

        db.commit()


main()
