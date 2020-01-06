#include "tetris.h"
#include <time.h>

static struct sigaction act, oact;
int score_number=0;
int B, count;
Node* head = NULL;

tNode *Root = NULL;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));
	time_t start, stop;
	double duration;
	start = time(NULL);
	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		case MENU_RANK: rank(); break; 
		case MENU_REC_PLAY:recommendedPlay(); break;
		default: break;
		}
	}
	stop = time(NULL);
	duration = (double)difftime(stop, start);
	printw("%lf seconds", duration);
	endwin();
	
	system("clear");
	return 0;
}

void InitTetris(int mode){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;
	
	start_color();

	for(i=1;i<8;i++)
		init_pair(i,i,0);

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	Root = (tNode *)malloc(sizeof(tNode));
	Root->level=0;
	Root->accumulatedScore =0;
	for( i=0;i<HEIGHT; i++)
		for(j=0; j<WIDTH ; j++){
			Root->recField[i][j] = field[i][j];
			Root->recField[i][j]= 0;
	}
	Root->curBlockID = nextBlock[0];
	Root->parent = NULL;
	Root->recBlockX = 0;
	Root->recBlockY = 0;
	Root->recBlockRotate=0;	
	setTree(Root);
	recommend(Root,0);
	
	if(mode ==1){
		blockY = Root->recBlockY;
		blockX = Root->recBlockX;
		blockRotate = Root->recBlockRotate;
	}
	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	removeTree(Root);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(10,WIDTH+10,4,8);
	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(16,WIDTH+10);
	printw("SCORE");
	DrawBox(17,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:

		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(18,WIDTH+11);
	printw("%8d",score);
}
void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}


   	for( i = 0; i < 4; i++ ){
                 move(11+i,WIDTH+13);
                 for( j = 0; j < 4; j++ ){
                         if( block[nextBlock[2]][0][i][j] == 1 ){
                                 attron(A_REVERSE);
                                 printw(" ");
                                 attroff(A_REVERSE);
                         }
                         else printw(" ");
                 }
         }

}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	//addch(ACS_ULCORNER);
	addch('o');
	for(i=0;i<width;i++)
		//addch(ACS_HLINE);
	//addch(ACS_URCORNER);
	addch('-');
	addch('o');
	for(j=0;j<height;j++){
		move(y+j+1,x);
		//addch(ACS_VLINE);
		addch('|');
		move(y+j+1,x+width+1);
		//addch(ACS_VLINE);
		addch('|');
	}
	move(y+j+1,x);
	//addch(ACS_LLCORNER);
	addch('o');
	for(i=0;i<width;i++)
		//addch(ACS_HLINE);
		addch('-');
	//addch(ACS_LRCORNER);
	addch('o');
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris(0);
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
	
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code

	int i, j;
	for (i=0;i< BLOCK_HEIGHT; i++)
	{
		for(j=0;j<BLOCK_WIDTH; j++)
		{
			if(block[currentBlock][blockRotate][i][j] == 1)		//1로 채워진 부분이
			{
				if( f[blockY+i][blockX+j] ==1)					//실제 좌표상 위치: blockY+i , blockX+j
					return 0;
				if( blockY+i >= HEIGHT)
					return 0;				
				if( blockX+j < 0)	
					return 0;				
				if( blockX+j >= WIDTH)
					return 0;		//그 값이 f[][]에 존재할 수 있는지 체크
			}
		}
	}
	return 1;	
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code

	int preBlockRotate, preBlockY, preBlockX;
			
		
									//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	preBlockX= blockX; preBlockY= blockY; preBlockRotate = blockRotate;
	switch(command){
	case KEY_UP:
		if(blockRotate ==0 )
			preBlockRotate = 3;
		else
		preBlockRotate = blockRotate-1;
		break;
	case KEY_DOWN:
		preBlockY = blockY-1;
		break;
	case KEY_RIGHT:
		preBlockX = blockX-1;
		break;
	case KEY_LEFT:
		preBlockX = blockX+1;
		break;
	default:
		break;
	}
	
									//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	int maxY = findmaxY(preBlockY, preBlockX, currentBlock, preBlockRotate);
  
	int i,j;
	for(i=0; i<4; i++)
	{
		for(j=0; j<4;j++){
			if(block[currentBlock][preBlockRotate][i][j] == 1)
			{
				move(i+preBlockY+1, j+preBlockX+1);			//커서 위치를 옮겨준다
				printw(".");
			}

		}
	}


         for(i=0; i<4; i++)
         {
                 for(j=0; j<4;j++){
                         if(block[currentBlock][preBlockRotate][i][j] == 1)
                         {
                                 move(i+maxY+1, j+preBlockX+1);                     //커서 위>    치를 옮겨준다
                                 printw(".");
                         }

                 }
         }



									//3. 새로운 블록 정보를 그린다. 

	DrawBlockWithFeatures(blockY,blockX, nextBlock[0], blockRotate);
	move(HEIGHT+10, WIDTH+10);
}

void BlockDown(int sig, int mode){
	//3주차 때 수정해야함
	//밑으로 한칸 내린다

	int i,j,x,y,r;
	if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX) ==1)
	{	blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0],blockRotate,blockY, blockX);	
		/*for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				if(block[nextBlock[0]][blockRotate][i][j] == 1)
				{
					move(i+blockY+1, j+blockX+1);
					printw(".");
				}
			}
		} 
	
		DrawBlock(++blockY, blockX, nextBlock[0], blockRotate, ' ');
*/
	}
	

	//블록이 필드에 도착하면
	else
	{
		//화면이 꽉차면 게임을 종료한다
		if(blockY == -1)
			gameOver = 1;

		else	
		{
			//블록을 필드에 추가한다.
			score += 10* AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
			score += DeleteLine(field);
			//다음 블록을 할당한다
			nextBlock[0] = nextBlock[1];			
			nextBlock[1] = nextBlock[2];
			nextBlock[2] = rand()%7;
			
			blockRotate = 0;
			blockY= -1;		
			blockX= WIDTH/2-2;
		
			Root->accumulatedScore =0;
			Root->level= 0;
			Root->recBlockX=0;
			Root->recBlockY=0;
			Root->recBlockRotate=0;
			Root->curBlockID= nextBlock[0];	
			//라인이 꽉차면 삭제한다
			
			//블록 초기화
			//필드가 수정되었으므로 ㅍㄹ드를 다시 그린다.
			
			DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
			//nextBlock을 다시 그린다.
			//DrawNextBlock(nextBlock);
			for(i=0;i<HEIGHT;i++)
				for(j=0;j<WIDTH;j++)
					Root->recField[i][j] = field[i][j];
			setTree(Root);
			recommend(Root,0);
			if(mode==1) 		//recommend play 모드면
			{
			x=recommendX; y= recommendY; r=recommendR;
			blockX=x; blockY=y; blockRotate=r;			
			blockX = Root->recBlockX;
			blockY = Root->recBlockY;
			blockRotate = Root->recBlockRotate;
			}
			DrawNextBlock(nextBlock);
			//스코어를 출력한다
			PrintScore(score);
			DrawField();
		}
	}
	timed_out=0;
	//강의자료 p26-27의 플로우차트를 참고한다.
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	
	int touched=0;
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(block[currentBlock][blockRotate][i][j] ==1){
				f[blockY+i][blockX+j]=1;
				if(blockY+i ==HEIGHT-1)
					touched++;
			}
		}
	}	
	return touched;
	//Block이 추가된 영역의 필드값을 바꾼다.
}


int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	int i,j;
	int flag =0;
	int deletedLine =0;
	int cnt =0;
	for(i=0;i<HEIGHT;i++)
	{
		for(j=0;j<WIDTH;j++)	
		{
			if(f[i][j] == 0)
			break;			
		}
		if(j==WIDTH)
		{	
			flag=1;
			deletedLine=i;
			cnt++;
	
			if(flag ==1){

                		for(i=deletedLine-1;i>0;i--)
                        		 for(j=0;j<WIDTH;j++)
                         	         { 
                        			 f[i+1][j]=f[i][j];
                         	         }
				flag=0;
        	 	}
		}
	}
	return cnt*cnt*100;
}
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	int maxY = findmaxY(y,x,blockID, blockRotate);
	DrawBlock(maxY, x, blockID, blockRotate, '/');
}
int findmaxY(int y, int x, int blockID, int blockRotate){
	int maxY =y;
         while(CheckToMove(field,blockID, blockRotate, maxY+1,x) )
                maxY++;
	return maxY;
}
void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawShadow(y,x,blockID, blockRotate);
	DrawRecommend(y,x,blockID, blockRotate);
	DrawBlock(y,x,blockID, blockRotate, ' ');
	
}

//3주차 --------------------------------------------------------------------------------------------------------


void createRankList(){
	// user code

	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE *fp;
	int i, j;
	//1. 파일 열기
	fp = fopen("rank.txt", "r");
 	//head = (Node *)malloc(sizeof(Node));
	head = NULL;
	// 2. 정보읽어오기
	/* int fscanf(FILE* stream, const char* format, ...);
	stream:데이터를 읽어올 스트림의 FILE 객체를 가리키는 파일포인터
	format: 형식지정자 등등
	변수의 주소: 포인터
	return: 성공할 경우, fscanf 함수는 읽어들인 데이터의 수를 리턴, 실패하면 EOF리턴 */
	// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문
	fscanf(fp, "%d", &score_number);
	char ** chData = (char**)malloc(sizeof(char*)*(score_number+1));
	int * scoreData = (int*)malloc(sizeof(int)*(score_number+1));
	for(i=0;i<score_number;i++)
	chData[i] = (char*)malloc(sizeof(char)*50);
	
	int dataIdx =0;	
	
	
	while(score_number>dataIdx && fscanf(fp, "%s %d", chData[dataIdx], &scoreData[dataIdx]) != EOF ){
		dataIdx++;
	}
	
		//3. LinkedList로 저장	
	Node* currentNode;
	for(i=0;i<dataIdx;i++){	
		Node* tempP = (Node*)malloc(sizeof(Node));
		tempP->score = scoreData[i];
		strcpy(tempP->name, chData[i]); 		
		tempP->link = NULL;
 
		if(head ==NULL){
			head= tempP;
			continue;
		}
		
		currentNode = head;
		while(1){
			if(currentNode->link ==NULL)
				break;
			else if(currentNode->score>tempP->score && currentNode->link->score <tempP->score){
				break; 		
			}
			else currentNode = currentNode->link;

		}
		
		tempP->link = currentNode->link;
		currentNode->link = tempP;
			
	}
		
	

	// 4. 파일닫기

	fclose(fp);
}

void rank(){
	// user code
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화
	createRankList();
	int X=1, Y=score_number, ch, i, j;
	int count =0;
	Node* p = head;
	clear();

	//2. printw()로 3개의 메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	
	

	//3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);

	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (ch == '1') {
		printw("X: ");
		echo();
		scanw("%d", &X);
		printw("Y: ");
		scanw("%d", &Y);
		noecho();
		printw("		name 		|	score 	\n");
		printw("-------------------------------------------------\n");
		if ( X>Y || score_number ==0 || X > score_number || Y> score_number)
			mvprintw(8 , 0 , "search failure: no rank in the list\n");
		else{	
			count =1;
			//채워넣기
			while(count != X){
			count++;
			p=p->link;
			}
			
			while(count != Y+1){
			printw("%s", p->name);
			//mvprintw(8,8, "|");
			printw("			    |	");
			printw("%d\n", p->score);
			count++;
			p=p->link;
			}
		}

	}

	//4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if ( ch == '2') {
		char str[NAMELEN+1];
		int len, tmplen;
		p = head;
		int flag =0;		
		printw("input the name: ");
		echo();
		scanw("%s", str);
		noecho();
		printw("		name 		|	score 	\n");
		printw("-------------------------------------------------\n");
		
		len = strlen(str);
		while(p!=NULL){
			tmplen = strlen(p->name);
			if(len==tmplen){
				for(i=0;i<len;i++){
					if(p->name[i] !=str[i])
						break;
				}
				if(i==len){
					printw("%s", p->name);
					printw("			    |	");
					printw("%d\n", p->score);
					flag = 1;
				}

			}
			p=p->link;
		}
		if(flag !=1)	
			mvprintw(8 , 0 , "search failure: no rank in the list\n");
	}
	//4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if ( ch == '3') {
		int num;
		int flag =0;
		p=head;
		Node *deleteNode;
		count =0;
		printw("input the rank: ");
		echo();
		scanw("%d", &num);
		noecho();
		if(num > score_number)	
			mvprintw(8 , 0 , "search failure: the rank not in the list\n");
		else{
			if(num ==1){
			deleteNode = p;
			head = p->link;
			free(deleteNode);
			score_number--;
			printw("result: the rank deleted");
			}
			else{
				while(1){
					count++;
					if(count > score_number){
					flag = -1;
					break;
					}
					if(count == num-1){
					deleteNode = p->link;
					if(p->link !=NULL)	
					p->link = p->link->link;
					else
					p->link = NULL;
					free(deleteNode);
					printw("result: the rank deleted ");
					score_number--;
					break;
					}				
					p=p->link;
				}
			}
			if(flag == -1)
			printw("search failure: the rank not in the list");
			else
			writeRankFile();
		}
	}
	getch();


	
}

void writeRankFile(){
	// user code
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	int sn, i;
	sn =0;
	//1. "rank.txt" 연다
	
	FILE *fp = fopen("rank.txt", "w");
	
	//2. 랭킹 정보들의 수를 "rank.txt"에 기록
	fprintf(fp, "%d\n", score_number);  
	Node* currentNode = head;
	//3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료
	while(1){
		if ( sn == score_number) break;
		else {	
			
			fprintf(fp, "%s %d\n", currentNode->name, currentNode->score);  
			sn++;
			currentNode = currentNode -> link;
		}
	}
/*	for ( i= 1; i < score_number+1 ; i++) {
		ifree(a.rank_name[i]);
	}
	free(a.rank_name);
	free(a.rank_score);

*/	
	fclose(fp);
}

void newRank(int score){
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char str[NAMELEN+1];
	int i, j;
	clear();
	//1. 사용자 이름을 입력받음
	printw("your name: ");
	echo();
	scanw("%s", str);
	noecho();	
	//2. 새로운 노드를 생성해 이름과 점수를 저장, score_number가
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->score = score;	strcpy(newNode->name, str);
	newNode->link =NULL;
	newNode->score = score;	strcpy(newNode->name, str);
	newNode->link =NULL;

	Node* currentNode= head;
	if(head == NULL ) {
		head = newNode;
	}
	else {
		while(1){
			if(currentNode->link == NULL)
				break;
			else if(currentNode->score > score && score > currentNode->link->score)
				break;
			else
				currentNode = currentNode->link;
		}
		
		newNode->link = currentNode->link;
		currentNode->link = newNode;
	}	
	score_number++;
	writeRankFile();	
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// DrawBlock을 이용한 추천 블록 그리기
	DrawBlock(Root->recBlockY,Root->recBlockX, Root->curBlockID, Root->recBlockRotate,'R');
}
int modified_recommend(tNode *root, int curLevel){
	int max=0;
	int i,j,k,l;
	int maxY;
	int tmpScore=0, tmpScore2=0;
	tNode **kid;
	setTree(root);
	kid = root->child;
	int kidIdx =0;
	if( curLevel > checkDepth-1)
		return 0;
	for(i=0;i<NUM_OF_ROTATE;i++)
	{
		if( (nextBlock[curLevel] ==0 || nextBlock[curLevel] ==4 || nextBlock[curLevel] == 5 || nextBlock[curLevel] ==6) && (i>=2))	continue;
		for(j=-1; j<WIDTH;j++)
		{	
			tmpScore=0;
			if(CheckToMove(root->recField, nextBlock[curLevel],i,blockY,j)){
				maxY= findmaxY(blockY, j,nextBlock[curLevel],i);		//maxY : 바닥에 닿는 y값
				for(k=0;k<HEIGHT;k++)	
					for(l=0;l<WIDTH;l++)
						kid[kidIdx]->recField[k][l]=root->recField[k][l];	//현재 block field 정보를 저장

				//block 정보를 저장
				kid[kidIdx]->recBlockX=j;	
				kid[kidIdx]->recBlockY=maxY;
				kid[kidIdx]->recBlockRotate=i;

				kid[kidIdx]->curBlockID=nextBlock[curLevel];
				kid[kidIdx]->accumulatedScore= root->accumulatedScore+AddBlockToField(kid[kidIdx]->recField,nextBlock[curLevel],i,maxY,j)+DeleteLine(kid[kidIdx]->recField);
				tmpScore = kid[kidIdx]->accumulatedScore;

				if(tmpScore>max)					//max setting
					max= tmpScore;
				if(curLevel== checkDepth-1)				//최대한 들어온거면
					max = kid[kidIdx]->accumulatedScore;		//max 최종 setting
				else{
					tmpScore2 = recommend(kid[kidIdx], curLevel+1);	//level 늘려서 recursive 호출
					if(tmpScore2 > max){				//들어가서 max 정보가 바뀌었다면
						max= tmpScore2;				//max update
						if(curLevel ==0){			//탐색을 모두 마쳤다면
							//root에 정보를 저장
							root->recBlockX = j;	
							root->recBlockY = maxY;
							root->recBlockRotate = i;
							root->curBlockID = nextBlock[curLevel];
							recommendX = j;
							recommendY = maxY;
							recommendR = i;
						}

					}
											
											//예외처리
					else if (tmpScore2 == max){			//들어갔는데 max 값이 그대로일 때
						if(root->recBlockY < maxY){		//root->recBlockY 값이 달라졌다면
							max = tmpScore2;
							if(curLevel ==0){
								root->recBlockX = j;	//root의 정보들을 다시 돌려놓는다.
								root->recBlockY = maxY;
								root->recBlockRotate = i;
								root->curBlockID = nextBlock[curLevel];
								recommendX = j;
								recommendY = maxY;
								recommendR = i;
							}
						}
					}
					kidIdx++;
					if(kidIdx == CHILDREN_MAX -1)
						break;
				}
			}
		}		
	}

	return max;
}	


		


int recommend(tNode *root, int curLevel){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// input : Node *root(parent노드의 주소를 저장하는 포인터)
	//output : tree의 최종 score, 블록 위치
	int i,j,k,l;
	int maxY;
	int tmpScore=0, tmpScore2=0;
	
	tNode **kid;
	setTree(root);				//root의 자식 생성
	kid=root->child;
	int kidIdx=0;
	if(curLevel > checkDepth -1)			//checkDepth: 미리 보는 블록의 단계수
		return 0;
	
	for(i=0;i<NUM_OF_ROTATE;i++)	
	{
		for(j=-1; j<WIDTH;j++)
		{	
			tmpScore=0;
			if(CheckToMove(root->recField, nextBlock[curLevel],i,blockY,j)){
				maxY= findmaxY(blockY, j,nextBlock[curLevel],i);		//maxY : 바닥에 닿는 y값
				for(k=0;k<HEIGHT;k++)	
					for(l=0;l<WIDTH;l++)
						kid[kidIdx]->recField[k][l]=root->recField[k][l];	//현재 block field 정보를 저장

				//block 정보를 저장
				kid[kidIdx]->recBlockX=j;	
				kid[kidIdx]->recBlockY=maxY;
				kid[kidIdx]->recBlockRotate=i;
				kid[kidIdx]->curBlockID=nextBlock[curLevel];
				kid[kidIdx]->accumulatedScore= root->accumulatedScore+AddBlockToField(kid[kidIdx]->recField,nextBlock[curLevel],i,maxY,j)+DeleteLine(kid[kidIdx]->recField);
				tmpScore = kid[kidIdx]->accumulatedScore;

				if(tmpScore>max)					//max setting
					max= tmpScore;
				if(curLevel== checkDepth-1)				//최대한 들어온거면
					max = kid[kidIdx]->accumulatedScore;		//max 최종 setting
				else{
					tmpScore2 = recommend(kid[kidIdx], curLevel+1);	//level 늘려서 recursive 호출
					if(tmpScore2 > max){				//들어가서 max 정보가 바뀌었다면
						max= tmpScore2;				//max update
						if(curLevel ==0){			//탐색을 모두 마쳤다면
							//root에 정보를 저장
							root->recBlockX = j;	
							root->recBlockY = maxY;
							root->recBlockRotate = i;
							root->curBlockID = nextBlock[curLevel];
							recommendX = j;
							recommendY = maxY;
							recommendR = i;
						}

					}
											
											//예외처리
					else if (tmpScore2 == max){			//들어갔는데 max 값이 그대로일 때
						if(root->recBlockY < maxY){		//root->recBlockY 값이 달라졌다면
							max = tmpScore2;
							if(curLevel ==0){
								root->recBlockX = j;	//root의 정보들을 다시 돌려놓는다.
								root->recBlockY = maxY;
								root->recBlockRotate = i;
								root->curBlockID = nextBlock[curLevel];
								recommendX = j;
								recommendY = maxY;
								recommendR = i;
							}
						}
					}
					kidIdx++;
					if(kidIdx == CHILDREN_MAX -1)
						break;
				}
			}
		}		
	}

	return max;
}
void recblockDown(int sig){
	BlockDown(sig, 1);		//blockDown에 flag 변수 1 을 넣어서 기존 blockDown을 변형 호출
}	
void recommendedPlay(){

	//play() 에서와 같이 초기 환경 세팅
	int command;
	int i,j;

	clear();
	act.sa_handler = recblockDown;	//blockDown 수정 버전
	sigaction(SIGALRM, &act, &oact);
	InitTetris(1); 			//InitTetris에 flag변수 1을 넣어서 기존 InitTetris를 변형 호출
			
	//play() 에서와 같이 같이 time_out 변수 관리, 입력받은 command 처리
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}
		command=wgetch(stdscr);			//사용자로부터 입력을 받으면
		switch(command){
			case 'q':			//q나 Q일 경우에아래 if문으로 이동
			case 'Q':
				command=QUIT;
				break;
			default:			//그 외에는 무시
				command=NOTHING;
				break;
		}

		if(ProcessCommand(command)==QUIT){	//q나 Q일 경우 게임을 종료
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();


}

void setTree(tNode* p)
{
	tNode **child = p->child;
	for(int i=0;i<CHILDREN_MAX;i++)
	{
		child[i]= (tNode*)malloc(sizeof(tNode));	//child 메모리 할당
		child[i]->level = p->level+1;			//child 초기화
		child[i]->parent = p;				//lv, 부모노드 연결
		if(child[i]->level < checkDepth-1)		
			setTree(child[i]);			//만들 수 있는 만큼 들어가서 자식 생성
	}
}
void removeTree(tNode* p)
{
	tNode **child = p->child;
	for(int i=0;i<CHILDREN_MAX;i++)
	{
		if(child[i]->level<checkDepth-1)
			removeTree(child[i]);
		free(child[i]);
	}
}





