#!/usr/bin/python3
import asyncio
import argparse
import subprocess
import csv
import os
import sys
from database import Database
from tasks import TasksAction
import compile_for
import reporters as reps


made_configs = set()


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Run benchmarker and insert results in SQLite file'
        )

    parser.add_argument(
            'tasks', metavar='config_id:benchmark_id|suite_id', nargs='*',
            default=[], action=TasksAction, help='Tasks to evaluate'
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

    parser.add_argument(
            '--dry', '-n', action='store_true',
            help='Print results instead of storing them to the database'
        )

    return parser.parse_args()


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
def benchmark(db, config_id, benchmark_id, use_stdout):

    # Gather benchmark information
    benchmark = db.get_by_id('benchmark', benchmark_id)

    if not benchmark:
        print('Error: Benchmark %s does not exist' % benchmark_id)
        return

    dataset = benchmark['dataset']
    reporters = reps.get(db, benchmark_id)

    if not reporters:
        print('No reporters to run for benchmark %s' % benchmark_id)
        return

    # Run the benchmark (async)
    process = yield from asyncio.create_subprocess_exec(
            *(
                    ['./bench', str(config_id), '../' + dataset] +
                    ['%(name)s:../%(arguments)s' % r for r in reporters]
                ),
            stdout=asyncio.subprocess.PIPE,
            stderr=sys.stdout if use_stdout else asyncio.subprocess.DEVNULL
        )

    status_code = yield from process.wait()
    if status_code != 0:
        print(
                "BENCHMARKER CRASHED!\n...when running: %s:%s" %
                (config_id, benchmark_id)
            )

    results = (yield from process.stdout.read()).decode('utf-8').split('\n\n')

    return zip(
            (list(csv.DictReader(r.split('\n'), delimiter='\t'))
                for r in results),
            reporters
        )


@asyncio.coroutine
def run_benchmark(db, task, use_stdout, dry):
    # Gather information
    commit = get_commit()
    (config_id, benchmark_id) = task

    # Build (if not already built)
    if config_id not in made_configs:
        print("Compiling for config %s..." % config_id)
        compile_for.compile(db, config_id)
        print("Config %s compiled" % config_id)

        made_configs.add(config_id)

    # Run the code
    results = yield from benchmark(db, config_id, benchmark_id, use_stdout)

    if dry:
        print('\n\n'.join(r[0] for r in results))
        return

    # Save results
    run_id = db.insert(
            'run',
            benchmark_id=benchmark_id,
            config_id=config_id,
            commit=commit
        ).lastrowid

    for result in results:
        db.insertmany(
                'result',
                (dict(r, run_id=run_id, reporter_id=result[1]['id'])
                    for r in results[0])
            )

    db.commit()


def main():
    args = parse_arguments()
    db = Database(args.database)
    build_dir = args.builddir

    # Prepare for out of source compilation
    subprocess.check_call(['mkdir', '-p', build_dir])
    os.chdir(build_dir)

    use_stdout = args.parallel == 1

    if not use_stdout:
        print(
                "\nNote: Parallel runs requested => no progress bars.\n"
            )

    # Run!
    run_queued(
            [run_benchmark(db, t, use_stdout, args.dry)
                for t in sorted(args.tasks)],
            max_parallel=args.parallel
        )


if __name__ == '__main__':
    main()
