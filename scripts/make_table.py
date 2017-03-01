#!/usr/bin/python3
from database import Database
import argparse


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Make a table similar to the one from the RR* article'
        )

    parser.add_argument(
            'index', metavar='<index name>',
            help='Index for which to generate the table'
        )

    parser.add_argument(
            '--database', '-d', metavar='filename', default='results',
            help='Path to sqlite database file'
        )

    return parser.parse_args()


def to_dict(results):
    return dict((r['name'], r['value']) for r in results)


def main():
    args = parse_arguments()
    db = Database(args.database)

    # Identify relevant runs
    benchmark_runs = db.connection.execute(
            """
            select `run`.*, `benchmark`.*
            from `config`
            inner join `benchmark` using (`config_id`)
            inner join `run` using (`benchmark_id`)
            where `config`.`index` = ?
                and not exists (
                    select * from `run` `r`
                    where `r`.`timestamp` > `run`.`timestamp`
                        and `r`.`benchmark_id` = `run`.`benchmark_id`
                )
            """,
            [args.index]
        ).fetchall()

    benchmark_runs.sort(key=lambda x: x['dataset'])

    values = ['benchmark'] \
        + 3 * ['results', 'leafAcc'] \
        + ['Leafs', 'Perim. splits']

    # TODO: Guarantee sort order?
    for run in benchmark_runs:
        values.append(run['dataset'].split('/')[-1])

        reporters = db.get_where(
                'reporter',
                benchmark_id=run['benchmark_id']
            )

        for reporter in reporters:
            results = to_dict(db.get_where(
                    'result',
                    run_id=run['run_id'],
                    reporter_id=reporter['reporter_id']
                ))

            if reporter['name'] == 'stats':
                values.extend([results['results'], results['leaf_accesses']])
            else:
                h = results['height']
                values.extend([
                    results['level_2'] / 1000,
                    100*results['perimeter_splits'] / (results['nodes'] - h)
                ])

    width = len(benchmark_runs) + 1
    height = len(values) // width

    for i in range(0, len(values) // width):
        print('\t'.join(str(v) for v in values[i::height]))


main()
