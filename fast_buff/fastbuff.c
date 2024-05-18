#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <vector>

using namespace std;

int mesgid = 1;

pthread_mutex_t mutex;
pthread_cond_t cond;

struct st_message
{
	int mesgid;
	char message[1024];
}stmesg;

vector<struct st_message> vcache;

void* outcache(void* arg)
{
	struct st_message stmesg;

	while (1) {
		pthread_mutex_lock(&mutex);
		while( vcache.size() == 0 ) {
			pthread_cond_wait(&cond,&mutex);
		}
		memcpy( &stmesg,&vcache[0],sizeof(struct st_message) );
		vcache.erase(vcache.begin());
		pthread_mutex_unlock(&mutex);
		printf("pthid=%ld,mesgid=%d\n",pthread_self(),stmesg.mesgid);
		usleep(100);
	}
}

void incache(int sig)
{
	struct st_message stmesg;
	memset(&stmesg,0,sizeof(struct st_message));
	pthread_mutex_lock(&mutex);
	//生产者，生产数据，放入缓存
	stmesg.mesgid = mesgid++; vcache.push_back(stmesg);
	stmesg.mesgid = mesgid++; vcache.push_back(stmesg);
	stmesg.mesgid = mesgid++; vcache.push_back(stmesg);
	stmesg.mesgid = mesgid++; vcache.push_back(stmesg);
	stmesg.mesgid = mesgid++; vcache.push_back(stmesg);
	pthread_mutex_unlock(&mutex);
	pthread_cond_broadcast(&cond);
}

int main()
{
	signal(15,incache);
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond,NULL);

	pthread_t pthid1,pthid2,pthid3;
	
	pthread_create(&pthid1,NULL,outcache,NULL);
	pthread_create(&pthid2,NULL,outcache,NULL);
	pthread_create(&pthid3,NULL,outcache,NULL);

	pthread_join(pthid1,NULL);
	pthread_join(pthid2,NULL);
	pthread_join(pthid3,NULL);
	return 0;
}

