#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#define NO 5
#define WORD_SIZE 100
#define SHORT_SIZE 30
#define LONG_SIZE 4
#define MAX_SIZE 1000000

#define ESC 0x1B
#define DEL 0x7F

#define WORD_ROUND 20
#define SHORT_ROUND 5

#define NUM_OF_PAGES 2
//긴 글 연습 데이터 저장을 위한 타입 정의
#define NUM_OF_ROWS 5

typedef struct _Article {
	char *page1[NUM_OF_ROWS];
	char *page2[NUM_OF_ROWS];
} Article;

void clear(void);
void move_cursor(int x, int y);
void start_msg(int no);
int getch(void);

int menu(void);
int cal_accuracy_for_short(char *str1, char *str2, int last);
int cal_speed_for_short(char *input, char *sen, int idx, struct timeval *start, struct timeval *end);

void exercise_pos(void);
void exercise_word(void);
void exercise_short(void);
void exercise_long(void);
void end(void);
void error(void);

int main(void) {
	int no;
	
	while(1) {
		no = menu();
	
		switch(no) {
		case 1:
			exercise_pos();
			break;
		case 2:
			exercise_word();
			break;
		case 3:
			exercise_short();
			break;
		case 4:
			exercise_long();
			break;
		case 5:
			end();
		default:
			error();
		}
	}
	
	return 0;
}

void clear(void) {
	system("clear");
}

void move_cursor(int x, int y) {
	printf("\e[%d;%df", y, x);
}

int getch(void) {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON|ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

void start_msg(int no) {
	char *msg[NO] = {"\b\b\b", "자리 연습", "낱말 연습", "짧은 글 연습", "긴 글 연습"};
	clear();
	move_cursor(0, 0);
	printf(">> 영문 타자 연습 프로그램 : %s <<\n", msg[no]);
}

int menu(void) {
	int no;
	start_msg(0);
	printf("1. 자리 연습\t");
	printf("2. 낱말 연습\n");
	printf("3. 짧은 글 연습\t");
	printf("4. 긴 글 연습\n");
	printf("5. 프로그램 종료\n\n");
	printf("번호를 선택하세요: ");
	scanf("%d", &no);
	getchar();
	return no;
}

void exercise_pos(void) {
    int right = 0; //정확히 입력한 단어 개수
    int pro = 0, err = 0, acc = 0; //pro:정확도 err:오타수 acc:정확도
    int input;
	char ch;
    bool is_uppercase;
	bool is_err = false;

	srand(time(NULL));
	while(right < 20) {
		start_msg(1);
		move_cursor(0, 2);
		printf("진행도 : %3d%%\t오타수 : %3d\t정확도 : %3d%%\n", pro, err, acc);

		if(!is_err) {
			is_uppercase = rand() % 2;// 대문자인지 소문자인지 rand함수로 결정
			if (!is_uppercase)// 소문자일때
				ch = rand() % 26 + 'a';// 소문자 랜덤
			else// 대문자 일때
				ch = rand() % 26 + 'A';// 대문자 랜덤
		}

		move_cursor(0, 5);
		printf("%c", ch);

		move_cursor(0, 6);
		input = getch();
		if(input == ESC)
			return;
		printf("%c", input);
		if(input == ch) {
			++right;
			pro += 5;
			is_err = false;
		}
		else {
			++err;
			is_err = true;
		}
		acc = right * 100 / (right + err);
	}

	start_msg(1);
	move_cursor(0,2);
	printf("진행도 : %3d%%\t오타수 : %2d\t정확도 : %3d%%\n", pro, err, acc);
	move_cursor(0, 5);
	printf("종료하려면 enter를 누르세요.");
	move_cursor(0, 6);
	while(getch() != '\n');
}

void exercise_word(void) {
	char *word[WORD_SIZE] = {
		"appointment", "communication", "doubt", "remind", "phenomenon", 
		"meanwhile", "import", "interacrion", "likewise", "logger",
		"munufacture", "maximize", "diminish", "renew", "physical", 
		"content", "extend", "ingredient", "gaze", "regular", 
		"minimize", "content", "transform", "intend", "expend",
		"official", "special", "combine", "reputation", "devote", 
		"gradual", "therefore", "class", "indeed", "require", 
		"comprehend", "debate", "announce", "structure", "indicate", 
		"resist", "exit", "resource", "function", "tough",
		"postpone", "outcome", "undergo", "preceive", "circular", 
		"hospital", "outstand", "expose", "enforce", "biography",
		"concern", "circumstance", "isolation", "analyze", "commute",
		"solve", "abstraction", "state", "common", "character", 
		"last", "perhaps", "matter", "privilege", "iron",
		"vanity", "fatigue", "calamity", "virtue", "impulse",
		"hyprocrisy", "system", "monotony", "conquest", "insight",
		"influence", "fortitude", "compromise", "prosperity", "comfort",
		"symphony", "candidate", "homage", "conceit", "frustration",
		"impression", "identity", "eloquence","adolescence", "heritage",
		 "add", "vivid", "simultaneous", "acquaintance", "ready"
	};
	int no;
	int match = 0;
	int progress = 0, typos = 0, accuracy = 0;
	int input;
	char input_buf[MAX_SIZE];
	int idx = 0, i, j;
	int selected_word[WORD_ROUND];
	bool duplicate;
	srand(time(NULL));

	for(i = 0; i < WORD_ROUND; i++) {
		start_msg(2);
		duplicate = false;
		while(1) {
			no = random() % WORD_SIZE;
			for(j = 0; j < i; j++) {
				if(selected_word[j] == no) {
					duplicate = true;
					break;
				}
				else
					duplicate = false;
			}
			if(!duplicate)
				break;
		}
		selected_word[i] = no;

		memset(input_buf, 0x00, MAX_SIZE);
		
		start_msg(2);
		move_cursor(0,2);
		printf("진행도 : %3d%%\t오타수 : %3d\t 정확도 : %d%%\n", progress, typos, accuracy);
		move_cursor(0, 5);
		printf("%s", word[no]);

		move_cursor(0,7);
		while (1) {
			input = getchar();
			if(input == '\n') {
				input_buf[idx] = 0;
				if(!strcmp("###", input_buf))
					return;
				if(strcmp(word[no], input_buf))
					typos++;
				else
					match++;

				accuracy = match * 100 / (typos + match);
				progress += 5;
				idx = 0;
				break;
			}
			else
				input_buf[idx++] = (char) input;
		}
	}

	if(i == WORD_ROUND) {
		start_msg(2);
		move_cursor(0,2);
		printf("진행도 : %3d%%\t오타수 : %3d\t 정확도 : %3d%%\n", progress, typos, accuracy);
		move_cursor(0, 5);
		printf("종료하려면 enter를 누르세요.");
		move_cursor(0,7);
	}
	while(getch() != '\n');
}

int cal_accuracy_for_short(char *input, char *sen, int last) {
	int correct_count = 0;
	int len = strlen(sen);
	int result;
	int i;
	if(last == 0)
		return 100;

	for(i = 0; i < last;  i++) {
		if(input[i] == sen[i])
			correct_count++;
	}

	result = last <= len ? correct_count * 100 / last : correct_count * 100 / len;
	return result;
}

int cal_speed_for_short(char *input, char *sen, int idx, struct timeval *start, struct timeval *end) {
	long long seconds = end->tv_sec - start->tv_sec;
	int speed;
	int i;
	int count_correct = 0;
	int last;
	int sen_len = strlen(sen);
	if(seconds == 0)
		return 0;
	
	last = idx > sen_len ? sen_len : idx;
	for(i = 0; i < last; i++) {
		if(input[i] == sen[i])
			count_correct++;
	}
	speed = (count_correct * 60) / seconds;
	return speed;	
}

void exercise_short(void) {
	char *sentence[SHORT_SIZE] = {
		"The grass is always greener on the other side of the fence.",//0
		"Don't judge a book by its cover.",//1
		"Strike while the iron is hot.",//2
		"Too many cooks spoil the broth.",//3
		"You can't have your cake and eat it too.",//4
		"Many hands make light work.",//5
		"When in Rome, do as the Romans do.",//6
		"Don't cross the bridge until you come to it.",//7
		"Honesty is the best policy.",//8
		"Practice makes perfect.",//9
		"Where there's a will, there's a way.",//10
		"Look before you leap.",//11
		"Beggars can't be choosers.",//12
		"Don't make a mountain out of an anthill.",//13
		"An apple a day keeps the doctor away.",//14
		"The early bird catches the worm.",//15
		"Better late than never.",//16
		"The cat is out of the bag.",//17
		"Two wrongs don't make a right.",//18
		"Always put your best foot forward.",//19
		"Rome wasn't built in a day.",//20
		"It's better to be safe than sorry.",//21
		"Don't bite the hand that feeds you.",//22
		"The squeaky wheel gets the grease.",//23
		"Don't bite off more than you can chew.",//24
		"You made your bed, now you have to lie in it.",//25
		"Actions speak louder than words.",//26
		"It takes two to tango.",//27
		"Don't count your chickens before they hatch.",//28
		"It's no use crying over spilled milk."//29
	};
	int no;
	int progress = 0, current_speed = 0, max_speed = 0, accuracy = 100;
	int input;
	char input_buf[MAX_SIZE];
	int idx = 0, i, j;
	bool duplicate;
	int selected_short[SHORT_ROUND];
	int round = SHORT_ROUND;
	bool is_first = true;

	struct timeval start, end;
	srand(time(NULL));

	for(i = 0; i < SHORT_ROUND; i++) {
		duplicate = false;
		while(1) {
			no = random() % SHORT_SIZE;
			for(j = 0; j < i; j++) {
				if(selected_short[j] == no) {
					duplicate = true;
					break;
				}
				else
					duplicate = false;
			}
			if(!duplicate)
				break;
		}
		selected_short[i] = no;
		memset(input_buf, 0x00, MAX_SIZE);
		while(1) {
			start_msg(3);
			move_cursor(0, 2);
			printf("진행도 : %3d%%\t현재타수 : %3d\t최고타수 : %3d\t정확도 : %d%%\n", progress, current_speed, max_speed, accuracy);
			move_cursor(0, 5);
			printf("%s", sentence[no]);

			move_cursor(idx, 7);
			input = getch();
			if(input == ESC) 
				return;
			else if(input == DEL) {
				if(idx <= 0) {
					continue;
				}
				idx--;
				accuracy = cal_accuracy_for_short(input_buf, sentence[no], idx);
				current_speed = cal_speed_for_short(input_buf, sentence[no], idx, &start, &end);
				max_speed = max_speed < current_speed ? current_speed : max_speed;
			}
			else if(input == '\n') {
				progress += 20;
				memset(input_buf, 0x00, MAX_SIZE);
				idx = 0;
				current_speed = 0;
				accuracy = 100;
				is_first = true;
				break;
			}
			else {
				if(is_first) {
 				 	gettimeofday(&start, NULL);
					is_first = false;
				}
				else
					gettimeofday(&end, NULL);
				input_buf[idx++] = input;
				accuracy = cal_accuracy_for_short(input_buf, sentence[no], idx);
				current_speed = cal_speed_for_short(input_buf, sentence[no], idx, &start, &end);
				max_speed = max_speed < current_speed ? current_speed : max_speed;
			}
			
			move_cursor(0, 7);
			input_buf[idx] = 0;
			printf("%s", input_buf);
		}	
	}

	start_msg(3);
	move_cursor(0, 2);
	printf("진행도 : %3d%%\t현재타수 : %3d\t최고타수 : %3d\t정확도 : %d%%\n", progress, current_speed, max_speed, accuracy);
	move_cursor(0, 5);
	printf("종료하려면 esc를 누르세요.");
	move_cursor(0, 7);

	while(getch() != ESC);
}

int cal_accuracy_for_long(char input_buf[][MAX_SIZE], Article *article, int page_num, int x_pos, int y_pos) {
	int i, j;
	int correct_count = 0;
	int len;
	int result;
	int total = 0;
	if(page_num == 1) {
		for(i = 0; i < 5; i++) {
			len = strlen(article->page2[i]);
			for(j = 0; j < len; j++) {
				total++;
				if(input_buf[i][j] == article->page2[i][j])
					correct_count++;
			}
		}

		for(i = 0; i < y_pos; i++) {
			len = strlen(article->page1[i]);
			for(j = 0; j < len; j++) {
				total++;
				if(input_buf[i][j] == article->page1[i][j])
					correct_count++;
			}
		}

		for(i = 0; i < x_pos; i++) {
			total++;
			if(input_buf[y_pos][i] == article->page1[y_pos][i])
				correct_count++;
		}
	}
	else {
		for(i = 0; i < y_pos; i++) {
			len = strlen(article->page1[i]);
			for(j = 0; j < len; j++) {
				total++;
				if(input_buf[i][j] == article->page1[i][j])
					correct_count++;
			}
		}

		for(i = 0; i < x_pos; i++) {
			total++;
			if(input_buf[y_pos][i] == article->page1[y_pos][i])
				correct_count++;
		}
	}

	if(total == 0)
		return 0;
	result = correct_count * 100 / total;
	return result;
}

int cal_speed_for_long(char input_buf[][MAX_SIZE], Article *article, int page_num, int x_pos, int y_pos,
						struct timeval *start, struct timeval *end) {
	long long seconds = end->tv_sec - start->tv_sec;
	int speed;
	int i, j;
	int correct_count = 0;
	int total = 0;
	int len;

	if(seconds == 0)
		return 0;
	
	if(page_num == 1) {
		for(i = 0; i < 5; i++) {
			len = strlen(article->page2[i]);
			for(j = 0; j < len; j++) {
				total++;
				if(input_buf[i][j] == article->page2[i][j])
					correct_count++;
			}
		}

		for(i = 0; i < y_pos; i++) {
			len = strlen(article->page1[i]);
			for(j = 0; j < len; j++) {
				total++;
				if(input_buf[i][j] == article->page1[i][j])
					correct_count++;
			}
		}

		for(i = 0; i < x_pos; i++) {
			total++;
			if(input_buf[y_pos][i] == article->page1[y_pos][i])
				correct_count++;
		}
	}
	else {
		for(i = 0; i < y_pos; i++) {
			len = strlen(article->page1[i]);
			for(j = 0; j < len; j++) {
				total++;
				if(input_buf[i][j] == article->page1[i][j])
					correct_count++;
			}
		}

		for(i = 0; i < x_pos; i++) {
			total++;
			if(input_buf[y_pos][i] == article->page1[y_pos][i])
				correct_count++;
		}
	}

	speed = (correct_count * 60) / seconds;
	return speed;	
}

void exercise_long(void) {
	unsigned offset = 12;
	Article article[LONG_SIZE] = {
		{//0
			{
				"The Selfish Giant",
				"Every afternoon, as they were coming from school, the children used",
				"to go and play in the Giant\'s garden.",
				"It was a large lovely garden, with soft green grass. Here and there",
				"over the grass stook beautiful flowers like stars, and there were",
			},
			{
				"twelve peachtrees that in the springtime broke out into delicate blos-",
				"soms of pink and pearl, and in the autumn bore rich fruit. The birds",
				"sat in the trees and sang so sweetly that the children used to stop",
				"their games in order to listen to them. \"How happy we are here!\" they",
				"cried to each other.",
			}
		},
		{//1
			{
				"The Elves and the Shoemaker",
				"There was once a shoemaker, who, through no fault of his own, became",
				"so poor that at last he had nothing left but just enough leather to",
				"make one pair of shoes. He cut out the shoes at night, so as to set to",
				"work upon them next morning; and as he had a good conscience, he laid",
			},
			{
				"himself quietly down in his bed, committed himself to heaven, and fell",
				"asleep. In the morning, after he had said his prayers, and was going",
				"to get to work, he found the pair of shoes made and finished, and stan-",
				"ding on his table. He wae very much astonished, and could not tell",
				"what to think, and he took the shoes in his hand to examine then more",
			}
		},
		{//2
			{
				"Rapunzel",
				"There once lived a man and his wife, who had long wished for a child,",
				"but in vain. Now there was at the back of their house a little window",
				"which overlooked a beautiful garden full of the finest vegetables and",
				"flowers; but there was a high wall all round it, and no one ventured",
			},
			{
				"into it, for it belonged to a witch of great might, and of whom all",
				"the world was afraid. One day, when the wife was standing at the win-",
				"dow, and looking into the garden, she saw a bed filled with the finest",
				"rampion; and it looked so fresh and green that she began to wish for",
				"some; and at length she longed for it greatly.",
			}
		},
		{//3
			{
				"The Wind and the Sun",
				"The North Wind was rushing along and blowing the clouds as he passed.",
				"\"Who is so strong as I?\" he cried.",
				"\"I am even stronger than the sun.\"",
				"\"Can you show that you are stronger?\" asked the Sun.",
			},
			{
				"\"A traveler is coming over the hill,\" said the Wind. \"Let us see which",
				"of us can first make him take off his long cloak.", 
				"The one who succeeds will prove himself the stronger.\"",
				"The North Wind began first. He blew a gale, tore up trees, and raised",
				"clouds of dust.",
			}
		}
	};
	size_t article_size[NUM_OF_PAGES][NUM_OF_ROWS];
	int no;
	int accuracy = 0, speed = 0;
	int input;
	char input_buf[NUM_OF_ROWS][MAX_SIZE] = { 0 };
	int page_num = 0;
	bool is_first = true;
	struct timeval start, end;
	int x_pos = 0, y_pos = 0;
	int i, j;

	srand(time(NULL));
	no = rand() % LONG_SIZE;

	for(i = 0; i < NUM_OF_ROWS; i++) {
		article_size[0][i] = strlen(article[no].page1[i]);
		article_size[1][i] = strlen(article[no].page2[i]);
	}

	while(1) {
		start_msg(4);
		move_cursor(0, 2);
		printf("정확도 : %3d%%\t현재타수 : %3d\n", accuracy, speed);
		
		move_cursor(0, 5);
		for(i = 0; i < NUM_OF_ROWS; i++) {
			if(page_num == 0)
				printf("%s\n", article[no].page1[i]);
			else 
				printf("%s\n", article[no].page2[i]);
		}
		
		move_cursor(0, offset);
		for(i = 0; i <= y_pos; i++)
			printf("%s\n", input_buf[i]);

		move_cursor(x_pos, y_pos + offset);
		input = getch();

		if(input == ESC)
			return;
		else if(input == DEL) {
			if(x_pos <= 0) {
				continue;
			}
			else {
				x_pos--;
				input_buf[y_pos][x_pos] = 0;
			}
			accuracy = cal_accuracy_for_long(input_buf, &article[no], page_num, x_pos, y_pos);
		}
		else {
			if(is_first) {
				gettimeofday(&start, NULL);
				is_first = false;
			}
			else
				gettimeofday(&end, NULL);
			
			if(x_pos == article_size[page_num][y_pos]) {
				x_pos = 0;
				y_pos++;
				if(input == ' ')
					continue;
			}
			
			if(input == '\n')
				continue;
			if(y_pos == 5) {
				if(page_num == 0) {
					page_num = 1;
					y_pos = 0;
				}
				else// page_num == 1인 경우
					break;
			}

			input_buf[y_pos][x_pos++] = input;
			input_buf[y_pos][x_pos] = 0;

			accuracy = cal_accuracy_for_long(input_buf, &article[no], page_num, x_pos, y_pos);
			speed = cal_speed_for_long(input_buf, &article[no], page_num, x_pos, y_pos, &start, &end);
		}
	}

	move_cursor(0, 2);
	printf("정확도 : %3d%%\t현재타수 : %3d\n", accuracy, speed);
	move_cursor(0, offset);

	while(getch() != ESC);
}

void end(void) {
	clear();
	exit(0);
}

void error(void) {
	fprintf(stderr, "잘못된 번호를 입력하였습니다.\n");
	sleep(1);
	clear();
}
