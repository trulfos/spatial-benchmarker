#!/usr/bin/python3
import asyncio
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

    parser.add_argument(
            '--parallel', '-p', metavar='num', default=1, type=int,
            help='Path to build dir'
        )

    return parser.parse_args()


def run_make(options, index):
    subprocess.check_call(
            ['cmake'] + ['-D%s=%s' % d for d in options.items()] + ['..'],
            stdout=subprocess.DEVNULL
        )

    subprocess.check_call(
            ['make', 'bench'],
            stdout=subprocess.DEVNULL
        )

    subprocess.check_call(
            ['make', index],
            stdout=subprocess.DEVNULL
        )


def run_queued(futures, max_parallel=1):
    """
    Run a set of futures in parallel.
    """
    loop = asyncio.get_event_loop()
    total = len(futures)
    outstanding = 0

    # Schedules next or stops loop if none left
    def schedule_next(future=None):
        nonlocal outstanding

        if future:
            print("Task done.")
            outstanding -= 1

        if len(futures) > 0:
            outstanding += 1

            print("Starting task %d of %d" % (total - len(futures) + 1, total))

            task = loop.create_task(futures.pop())
            task.add_done_callback(schedule_next)

        if outstanding == 0:
            loop.stop()

    # Schedule first set of tasks
    for _ in range(0, min(max_parallel, len(futures))):
        schedule_next()

    loop.run_forever()
    loop.close()


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


@asyncio.coroutine
def run_benchmark(db, benchmark_id):
    # Gather information
    commit = get_commit()
    benchmark = db.get_by_id('benchmark', benchmark_id)

    if not benchmark:
        print('Error: Benchmark %s does not exist' % benchmark_id)
        return

    config_id = benchmark['config_id']
    config = configs.get_by_id(db, config_id)
    dataset = benchmark['dataset']
    reporters = reps.get(db, benchmark_id)

    if not reporters:
        print('No reporters to run for benchmark %d' % benchmark_id)
        return

    dimension = detect_dimension(dataset)
    index = config['index']

    # Build
    definitions = dict(config['definitions'], D=dimension)
    run_make(definitions, index)

    # Copy lib to avoid overwriting before load
    library = '%s_%d' % (index, config['id'])
    os.rename('lib%s.so' % index, 'lib%s.so' % library)

    # Run the benchmark (async)
    process = yield from asyncio.create_subprocess_exec(
            *(
                    ['./bench', library, '../' + dataset] +
                    ['%(name)s:../%(arguments)s' % r for r in reporters]
                ),
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE
        )

    status_code = yield from process.wait()
    if status_code != 0:
        print("BENCHMARKER CRASHED!\nBenchmark: %s" % benchmark_id)

    results = (yield from process.stdout.read()).decode('utf-8')

    # Save results
    run_id = db.insert(
            'run', benchmark_id=benchmark_id, commit=commit
        ).lastrowid

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

    # Run!
    run_queued(
            [run_benchmark(db, b) for b in benchmarks],
            max_parallel=args.parallel
        )


main()
