#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#define NR_WP 32

static WP wp_list[NR_WP];
static WP *head, *free_;
void init_wp_list() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}

/* TODO: Implement the functionality of watchpoint */

bool new_wp(char *e){
/*	bool success=true;
	uint32_t ans;
	WP* find=free_;
	ans=expr(e,&success);
	find->ans=ans;
	find->expr=e;
	free_=free_->next;
	if(!success){
		printf("EXPR WRONG!\n");
		return false;
	}
	if(free_==NULL){
    	printf("There is no free watchpoints\n");
		return false;
	}
	if(head==NULL){
		head=find;
		find->next=NULL;
		printf("Watchpoint %d at %s\n",find->NO,find->expr);
		return true;
	}
	else{
	WP *search;
	for(search=head;;search=search->next){
		if(search->next==NULL){
			break;
		}
	}
	search->next=find;
	find->next=NULL;
	printf("Watchpoint %d at %s\n",find->NO,find->expr);
	return true;}*/
	if(strlen(e)>=30){
		printf("The length of expr exceed!\n");
		return false;
	}
	bool success=true;
	int i;
	uint32_t temp;
	temp=expr(e,&success);
	if(!success){
		printf("EXPR ERROE at watchpoint!\n");
		return false;
	}
	if(free_==NULL){
		printf("There is no free watchpoints!\n");
		return false;
	}
	WP *find=free_;
	free_=free_->next;
	for(i=0;i<strlen(e);i++){
		find->expr[i]=e[i];
	}
	find->expr[i]='\0';
	find->ans=temp;
	find->next=head;
	head=find;
	printf("Set watchpoint %d at %s\n",head->NO,head->expr);
	return true;
	


}
WP* search_NO(uint32_t num){
	bool flag=false;
	if(head==NULL){
		return NULL;
	}
	WP* work;
	for(work=head;work!=NULL;work=work->next){
		if(work->NO==num){
			flag=true;
			break;
		}
	}
	if(flag==true){
		return work;
	}
	else{
		return NULL;
	}
}
void free_wp(WP *wp){
	if(wp==head){
		head=head->next;
		if(free_==NULL){
			free_=wp;
			free_->next=NULL;
			printf("Delete the watchpoint %d at %s\n",wp->NO,wp->expr);
			return ;
		}
		else{
			WP *search2;
			for(search2=free_;;search2=search2->next){
				if(search2->next==NULL){
					break;
				}
			}
			int num;
			printf("Delete the watchpoint %d at %s\n",wp->NO,wp->expr);
			num=search2->NO;
			wp->NO=num+1;
			search2->next=wp;
			wp->next=NULL;
			return ;
		}

	}
	WP *search1;
	WP *search2;
	int num;
	for(search1=head;;search1=search1->next){
		if(search1->next==wp){
			break;
		}
	}
	search1->next=wp->next;
	for(search2=free_;;search2=search2->next){
		if(search2->next==NULL){
			break;
		}
	}
	printf("Delete the watchpoint %d at %s\n",wp->NO,wp->expr);
	num=search2->NO;
	wp->NO=num+1;
	search2->next=wp;
	wp->next=NULL;
	return ;
}
bool check_watchpoint(){
	bool flag=true;
	if(head==NULL){
	//	printf("There are no watchpoints to check!\n");
		flag=true;
	//	assert(0);
		return flag;
	}
	else{
		WP *work;
		char *temp;
		bool success=true;
		uint32_t temp_ans;
		for(work=head;work!=NULL;work=work->next){
			if(work==NULL) break;
			temp=work->expr;
			temp_ans=expr(temp,&success);
			if(temp_ans!=work->ans){
				printf("Stop at the %d watchpoint!\n",work->NO);
				printf("The ans changes from %u to %u!",work->ans,temp_ans);
				printf("The expr of this watchpoint is %s\n",work->expr);
				printf("\n");
				work->ans=temp_ans;
				flag=false;
			}
		}
		return flag;
	}
}
void watchpoint_info(){
	WP* work;
	if(head==NULL){
		printf("There are no watchpoints to print!\n");
		return ;
	}
	else{
		for(work=head;work!=NULL;work=work->next){
			printf("Watch point %d at %s\n",work->NO,work->expr);
		}
		return ;
	}
}



	





