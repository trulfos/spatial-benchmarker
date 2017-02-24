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
            'configs', nargs='+',
            help='Configurations to benchmark (ids)'
        )

    parser.add_argument(
            '--database', '-d', metavar='filename', default='results',
            help='Path to sqlite database file'
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


def detect_dimension(filename):
    i = len(filename) - 1

    while filename[i].isdigit() and i > 0:
        i -= 1

    return int(filename[i + 1:])


def main():
    args = parse_arguments()
    db = Database(args.database)
    build_dir = 'build'

    for config_id in set(args.configs):

        # Prepare for out of source compilation
        subprocess.check_call(['mkdir', '-p', build_dir])
        os.chdir(build_dir)

        # Gather information
        commit = get_commit()
        config = configs.get_by_id(db, config_id)

        if not config:
            print('Error: Config %s does not exist' % config_id)
            continue

        reporters = reps.get(db, config_id)

        if not reporters:
            print('No reporters to run for config %d' % config_id)
            continue

        dimension = detect_dimension(config['data'])

        # Build
        definitions = dict(config['definitions'], D=dimension)
        run_make(definitions, config['index'])

        # Run the bencmark
        results = subprocess.check_output(
                [
                    './bench',
                    config['index'],
                    '../' + config['data'],
                ] + ['%(name)s:%(arguments)s' % r for r in reporters]
            ).decode('utf-8')

        # Save results
        for result in zip(results.split('\n\n'), reporters):
            results_reader = csv.DictReader(
                    result[0].split('\n'),
                    delimiter='\t'
                )

            run_id = db.insert(
                    'run', config_id=config_id, commit=commit
                ).lastrowid

            db.insertmany(
                    'result',
                    (dict(r, run_id=run_id, reporter_id=result[1]['id'])
                        for r in results_reader)
                )

        db.commit()


main()
