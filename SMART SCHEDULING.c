#include <stdio.h>
#include <limits.h>
#define MAX 100

typedef struct {
int at, bt, pr;
} Process;
const char *names[] = {"FCFS", "SJF", "Priority", "Round Robin"};

/* Modes 0-2 are non-preemptive; mode 3 uses a FIFO ready queue. */
double schedule(const Process p[], int n, int mode, int quantum) {
	int ct[MAX]= {0}, rem[MAX], order[MAX], queue[MAX+1];
	int time=0, done=0, next=0, head=0, tail=0;
	double sw=0, st=0;
	for(int i=0; i<n; i++) {
		rem[i]=p[i].bt;
		order[i]=i;
	}
	/* Stable arrival-time ordering. */
	for(int i=1; i<n; i++) {
		int k=order[i], j=i;
		while(j>0 && p[order[j-1]].at>p[k].at) {
			order[j]=order[j-1];
			j--;
		}
		order[j]=k;
	}
	while(done<n) {
		int k=-1;
		if(mode==3) {
			if(head==tail && next<n && time<p[order[next]].at)
				time=p[order[next]].at;
			while(next<n && p[order[next]].at<=time) {
				queue[tail]=order[next++];
				tail=(tail+1)%(MAX+1);
			}
			k=queue[head];
			head=(head+1)%(MAX+1);
			int run=rem[k]<quantum?rem[k]:quantum;
			time+=run;
			rem[k]-=run;
			while(next<n && p[order[next]].at<=time) {
				queue[tail]=order[next++];
				tail=(tail+1)%(MAX+1);
			}
			if(rem[k]) {
				queue[tail]=k;
				tail=(tail+1)%(MAX+1);
			}
		} else {
			int future=INT_MAX;
			for(int j=0; j<n; j++) {
				int i=order[j];
				if(!rem[i])continue;
				if(p[i].at>time) {
					if(p[i].at<future)future=p[i].at;
					continue;
				}
				if(k<0 || (mode==1 && p[i].bt<p[k].bt) ||
				        (mode==2 && p[i].pr<p[k].pr))k=i;
			}
			if(k<0) {
				time=future;
				continue;
			}
			time+=rem[k];
			rem[k]=0;
		}
		if(!rem[k]) {
			ct[k]=time;
			done++;
		}
	}
	printf("\n%s Scheduling\nPID\tAT\tBT\tPR\tCT\tTAT\tWT\n",names[mode]);
	for(int i=0; i<n; i++) {
		int tat=ct[i]-p[i].at, wt=tat-p[i].bt;
		sw+=wt;
		st+=tat;
		printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n",i+1,p[i].at,p[i].bt,p[i].pr,ct[i],tat,wt);
	}
	printf("Average waiting time: %.2f\nAverage turnaround time: %.2f\n",sw/n,st/n);
	return sw/n;
}
int main(void) {
	Process p[MAX];
	int n,q,choice;
	printf("SMART JOB SCHEDULING SYSTEM\nNumber of processes (1-100): ");
	if(scanf("%d",&n)!=1 || n<1 || n>MAX)return 1;
	for(int i=0; i<n; i++) {
		printf("P%d arrival, burst, priority (lower number = higher priority): ",i+1);
		if(scanf("%d%d%d",&p[i].at,&p[i].bt,&p[i].pr)!=3 || p[i].at<0 || p[i].at>1000000 || p[i].bt<1 || p[i].bt>1000000 || p[i].pr<0)return 1;
	}
	printf("Round Robin time quantum: ");
	if(scanf("%d",&q)!=1 || q<1)return 1;
	for(;;) {
		printf("\n1.FCFS  2.SJF  3.Priority  4.Round Robin  5.Compare all  0.Exit\nChoice: ");
		if(scanf("%d",&choice)!=1 || choice==0)break;
		if(choice>=1 && choice<=4)schedule(p,n,choice-1,q);
		else if(choice==5) {
			double a[4],best;
			for(int i=0; i<4; i++)a[i]=schedule(p,n,i,q);
			best=a[0];
			for(int i=1; i<4; i++)if(a[i]<best)best=a[i];
			printf("\nRecommended (minimum average waiting time %.2f): ",best);
			for(int i=0; i<4; i++)if(a[i]-best<1e-9)printf("%s; ",names[i]);
			puts("");
		} else puts("Invalid choice.");
	}
	return 0;
}