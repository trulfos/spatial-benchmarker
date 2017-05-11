#!/usr/bin/python3

"""
This script investigates the impact of a config param X on a result Y.

Note that the average value of the statistic is reported by default. This
obviously makes no difference when the statistic is only reported once.
"""

from database import Database
import argparse


def group_by(values, **kwargs):
    keyfunc = kwargs['key'] if 'key' in kwargs else (lambda x: x)
    groups = {}

    for v in values:
        key = keyfunc(v)

        if key not in groups:
            groups[key] = []

        groups[key].append(v)

    return groups


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Print a statistic as a function of a config parameter'
        )

    parser.add_argument(
            'config', metavar='<config parameter>',
            help='Configuration parameter to investigate'
        )

    parser.add_argument(
            'metric', metavar='<metric>',
            help='Metric to print'
        )

    parser.add_argument(
            'suite', metavar='<suite id>', nargs='+',
            help='Limit results to the given suites'
        )

    parser.add_argument(
            '--database', '-d', metavar='filename', default='results',
            help='Path to sqlite database file'
        )

    return parser.parse_args()


def print_warning(message):
    print('\033[31mWarning: %s\033[0m' % message)


def main():
    args = parse_arguments()
    db = Database(args.database)

    results = db.connection.execute(
            """
            select
                `commit`,
                `index`,
                `benchmark_id`,
                `reporter_id`,
                `option`.`value` `option`,
                avg(`result`.`value`) `value`,
                stddev_samp(`result`.`value`) `stddev`,
                count(distinct `config_id`) `config_count`

            from `latest_run`
            inner join `result` using (`run_id`)
            inner join `option` using (`config_id`)
            inner join `suite_member` using (`config_id`, `benchmark_id`)

            where `result`.`name` = ?
                and `option`.`name` = ?
                and `suite_id` in (%s)

            group by
                `index`,
                `benchmark_id`,
                `reporter_id`,
                `option`.`value`

            order by
                `index`,
                cast(`option`.`value` as real),
                `option`.`value`,
                `benchmark_id`,
                `reporter_id`
            """ % ', '.join(['?'] * len(args.suite)),
            (args.metric, args.config) + tuple(args.suite)
        ).fetchall()

    for r in results:
        print(r)

    by_index = group_by(results, key=lambda r: r['index'])

    for index, index_data in by_index.items():
        print("\n----- %s -----" % index)

        if max(d['config_count'] for d in index_data) > 1:
            print_warning('Several different configs have been averaged!')

        if len(set(d['commit'] for d in index_data)) > 1:
            print_warning('Data from several commits have been combined!')

        print([d['option'] for d in index_data])

        # Determine keys?
        keys = sorted(
                set((d['benchmark_id'], d['reporter_id']) for d in index_data)
            )

        print(
                'benchmark_id\treporter_id\t' +
                '\t'.join('%s-%s' % k + '\tstddev' for k in keys)
            )

        by_option = group_by(
                index_data,
                key=lambda d: d['option']
            )

        for option, values in by_option.items():
            print(option, end='\t')

            vs = dict(
                    group_by(
                        list(values),
                        key=lambda v: (v['benchmark_id'], v['reporter_id'])
                    )
                )

            print(
                    '\t'.join(
                            '%(value)s\t%(stddev)s' % dict(vs[k][0])
                            if k in vs else ''
                            for k in keys
                        )
                    )


if __name__ == '__main__':
    main()
