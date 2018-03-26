#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/delay.h>

/* www.tldp.org/LDP/lkmpg/2.6/html/x769.html */
/* fs/proc/stat.c */
#include <linux/cpumask.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/irqnr.h>
#include <asm/cputime.h>

#ifndef arch_irq_stat_cpu
#define arch_irq_stat_cpu(cpu) 0
#endif
#ifndef arch_irq_stat
#define arch_irq_stat() 0
#endif
#ifndef arch_idle_time
#define arch_idle_time(cpu) 0
#endif

static void sys_CPU_sample(unsigned long long *total_usage, unsigned long long *total)
{
	int i, j;
	unsigned long jif;
	cputime64_t user, nice, system, idle, iowait, irq, softirq, steal;
	cputime64_t guest;
	u64 sum = 0;
	u64 sum_softirq = 0;
	unsigned int per_softirq_sums[NR_SOFTIRQS] = {0};
	struct timespec boottime;
	unsigned int per_irq_sum;

	user = nice = system = idle = iowait =
		irq = softirq = steal = cputime64_zero;
	guest = cputime64_zero;
	getboottime(&boottime);
	jif = boottime.tv_sec;

	for_each_possible_cpu(i) {
		user = cputime64_add(user, kstat_cpu(i).cpustat.user);
		nice = cputime64_add(nice, kstat_cpu(i).cpustat.nice);
		system = cputime64_add(system, kstat_cpu(i).cpustat.system);
		idle = cputime64_add(idle, kstat_cpu(i).cpustat.idle);
		idle = cputime64_add(idle, arch_idle_time(i));
		iowait = cputime64_add(iowait, kstat_cpu(i).cpustat.iowait);
		irq = cputime64_add(irq, kstat_cpu(i).cpustat.irq);
		softirq = cputime64_add(softirq, kstat_cpu(i).cpustat.softirq);
		steal = cputime64_add(steal, kstat_cpu(i).cpustat.steal);
		guest = cputime64_add(guest, kstat_cpu(i).cpustat.guest);
		for_each_irq_nr(j) {
			sum += kstat_irqs_cpu(j, i);
		}
		sum += arch_irq_stat_cpu(i);

		for (j = 0; j < NR_SOFTIRQS; j++) {
			unsigned int softirq_stat = kstat_softirqs_cpu(j, i);

			per_softirq_sums[j] += softirq_stat;
			sum_softirq += softirq_stat;
		}
	}
	sum += arch_irq_stat();

	*total_usage =
		(unsigned long long)cputime64_to_clock_t(user) +
		(unsigned long long)cputime64_to_clock_t(nice) + 
		(unsigned long long)cputime64_to_clock_t(system) +
		(unsigned long long)cputime64_to_clock_t(irq) +
		(unsigned long long)cputime64_to_clock_t(softirq) +
		(unsigned long long)cputime64_to_clock_t(steal) +
		(unsigned long long)cputime64_to_clock_t(guest);
	*total = *total_usage +
		(unsigned long long)cputime64_to_clock_t(idle) +
		(unsigned long long)cputime64_to_clock_t(iowait);
}

asmlinkage unsigned long sys_CPU(void)
{
	unsigned long long usage1, total1, usage2, total2;
	unsigned long delta_usage, delta_total, delta_idle;
	unsigned long utilization_percentage;
	sys_CPU_sample(&usage1, &total1);
	mdelay(2000);
	sys_CPU_sample(&usage2, &total2);
	delta_usage = (unsigned long)usage2 - (unsigned long)usage1;
	delta_total = (unsigned long)total2 - (unsigned long)total1;
	delta_idle = delta_total - delta_usage;
	utilization_percentage = (delta_usage * 100) / delta_total;
        return utilization_percentage;
}

