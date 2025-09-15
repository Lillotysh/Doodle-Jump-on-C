#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

//zalupa

struct stairs
{
    int x;
    float y;
};

struct doodle_c
{
    int x;
    int y;
};

struct stairs_data {
    struct stairs *stairs_array;
    WINDOW *win;
};

struct doodle_data {
    struct doodle_c doodle;
    
    WINDOW *win;
    int key;
};

WINDOW *create_win();
void delay(float seconds);
void generate_stairs(struct stairs stairs_array[], int count, int width, int height);
void stairs_up_refresh(struct stairs_data *stairs_data);
void stairs_up(struct stairs_data *stairs_data);
void stairs_down_refresh(struct stairs_data *stairs_data);
void stairs_down(struct stairs_data *stairs_data);
void doodle_move(struct doodle_data *doodle_data);
int checker(int x, struct stairs_data *stairs_data);
int stairs_checker(struct stairs_data *stairs_data);
void print_lose(WINDOW *win);
int main_print(WINDOW *win);
int main_print_2(WINDOW *win);


int main(int argc, char *argv[])
{
    
    WINDOW *win;
    noecho();
    curs_set(0);
    win = create_win();
    curs_set(0);
    
    keypad(win, TRUE);
    keypad(stdscr, TRUE);
    refresh();
    

    struct stairs stairs_array[25];
    generate_stairs(stairs_array, 25, 30, 25);
    struct doodle_c doodle = {15, 15};
    struct doodle_data doodle_data = {doodle ,win, 0};
    struct stairs_data stairs_data = {stairs_array, win};

    box(win, 0 , 0);
    int score = 0;
    int i = 100;
    int inertia = 1000000;
    int c = 0;
    int choice = 0;
    int ch = 0;

    mvwprintw(win, 23, 1, "Press any key up or key down");
    int wh = 1;
    while ((ch = wgetch(win)) != 'q' && wh == 1) {
        switch (ch) {
            case KEY_UP:
                main_print(win);
                choice = 1;
                break;
            case KEY_DOWN:
                main_print_2(win);
                choice = 2;
                break;
            case '\n':
                if (choice == 2) {
                    endwin();
                    return 0;
                    break;
                } 
                else if (choice == 1) 
                {
                    wh = 0;
                    break;
                }
        }
    }
    nodelay(stdscr, TRUE); 
    stairs_up(&stairs_data);
    while (1)
    {
        
        werase(win);
        if (checker(doodle_data.doodle.x, &stairs_data))
        {
            
            inertia = 100;
            i = 0;
            while (1)
            {
                werase(win);
                if (i >= inertia)
                {
                    stairs_up_refresh(&stairs_data);
                    score++;
                    mvwprintw(win, 1,1,"%d", score);
                    if (inertia * 1.7 <= 999999 )
                    {
                        inertia *= 1.7;
                    }
                    else break;
                    i = 0;
                }
                else
                {
                    stairs_up(&stairs_data);
                    mvwprintw(win, 1,1,"%d", score);
                }
                i += 10;
                box(win, 0, 0);
                mvwprintw(win, doodle_data.doodle.y, doodle_data.doodle.x, "^");
                wrefresh(win);
                c = getch();       
                if (c != ERR) {       
                    if (c == KEY_LEFT || c == KEY_RIGHT) {
                        doodle_data.key = c;
                        doodle_move(&doodle_data);
                    }
                }
            }
        }
        
        if (i >= inertia)
        {
            stairs_down_refresh(&stairs_data);
            mvwprintw(win, 1,1,"%d", score);
            if (inertia / 2 >= 100000 )
            {
                inertia /= 1.3;
            }
            i = 0;
        }
        else
        {
            stairs_down(&stairs_data);
            mvwprintw(win, 1,1,"%d", score);
        }
        box(win, 0, 0);
        mvwprintw(win, doodle_data.doodle.y, doodle_data.doodle.x, "^");
        wrefresh(win);
        c = getch();       
        if (c != ERR) {       
            if (c == KEY_LEFT || c == KEY_RIGHT) {
                doodle_data.key = c;
                doodle_move(&doodle_data);
            }
        }
        i += 10;

        if (stairs_checker(&stairs_data))
        {
            break;
        }
    }
    nodelay(stdscr, FALSE); 
    print_lose(win);
    mvwprintw(win, 23, 1, "Your score is %d", score);
    wrefresh(win);
    refresh();
    getch();

    endwin();
    return 0;
}

WINDOW *create_win()
{   WINDOW *my_win;
	int startx, starty, width, height;
	int ch;

	initscr();			
	cbreak();	
    noecho();	
    
	height = 25;
	width = 30;
	starty = (LINES - height) / 2;
	startx = (COLS - width) / 2;
	my_win = newwin(height, width, starty, startx);
    box(my_win, 0 , 0);
    wrefresh(my_win);
	getch();
    return my_win;
}

void delay(float seconds) 
{
    int micro_seconds = (int)(seconds * 1000000);
    usleep(micro_seconds);
}

void stairs_up_refresh(struct stairs_data *stairs_data)
{
    
    struct stairs_data *data = stairs_data;
    struct stairs *stairs_array = data->stairs_array;    
    WINDOW *win = data->win; 
    
    werase(win);
    for (int j = 0; j < 10; j++)
    {
        if (fmod(stairs_array[j].y, 1.0) == 0.0)
        {
            mvwprintw(win, stairs_array[j].y, stairs_array[j].x, "____");
            stairs_array[j].y += 0.5;
        }
        else
        {
            mvwprintw(win, stairs_array[j].y + 1, stairs_array[j].x, "----");
            stairs_array[j].y += 0.5;
        }
            
        if (stairs_array[j].y == 24)
        {
            stairs_array[j].y = 1;
            stairs_array[j].x = rand() % 24 + 1;
        }
    }
    box(win, 0 , 0);
}

void stairs_up(struct stairs_data *stairs_data)
{
    
    struct stairs_data *data = stairs_data;
    struct stairs *stairs_array = data->stairs_array;    
    WINDOW *win = data->win; 
    
    werase(win);
    for (int j = 0; j < 10; j++)
    {
        if (fmod(stairs_array[j].y, 1.0) == 0.0)
        {
            mvwprintw(win, stairs_array[j].y, stairs_array[j].x, "____");
        }
        else
        {
            mvwprintw(win, stairs_array[j].y + 1, stairs_array[j].x, "----");
        }
    }
    box(win, 0 , 0);
}

void stairs_down_refresh(struct stairs_data *stairs_data)
{
    struct stairs_data *data = stairs_data;
    struct stairs *stairs_array = data->stairs_array;    
    WINDOW *win = data->win; 

    werase(win);
    for (int j = 0; j < 10; j++)
    {
        if (fmod(stairs_array[j].y, 1.0) == 0.0)
        {
            mvwprintw(win, stairs_array[j].y, stairs_array[j].x, "____");
            stairs_array[j].y -= 0.5;
        }
        else
        {
            mvwprintw(win, stairs_array[j].y + 1, stairs_array[j].x, "----");
            stairs_array[j].y -= 0.5;
        }
    }

    box(win, 0 , 0);
}

void stairs_down(struct stairs_data *stairs_data)
{
    struct stairs_data *data = stairs_data;
    struct stairs *stairs_array = data->stairs_array;    
    WINDOW *win = data->win; 

    werase(win);
    for (int j = 0; j < 10; j++)
    {
        if (fmod(stairs_array[j].y, 1.0) == 0.0)
        {
            mvwprintw(win, stairs_array[j].y, stairs_array[j].x, "____");
        }
        else
        {
            mvwprintw(win, stairs_array[j].y + 1, stairs_array[j].x, "----");
        }
    }

    box(win, 0 , 0);
}


void generate_stairs(struct stairs stairs_array[], int count, int width, int height) 
{
    srand(time(NULL));

    for (int i = 0; i < count; i++) {
        stairs_array[i].x = rand() % 24 + 1; 
        stairs_array[i].y = (rand() % 24) + 1;  
    }
}

void doodle_move(struct doodle_data *doodle_data)
{
    struct doodle_data *data = doodle_data;
 
    struct doodle_c *doodle = &data->doodle; 

    switch (data->key)
    {
    case KEY_LEFT:
        --doodle->x;
        break;
    
    case KEY_RIGHT:
        ++doodle->x;
        break;
    }

}

int checker(int x, struct stairs_data *stairs_data)
{
    struct stairs_data *data = stairs_data;
    struct stairs *stairs_array = data->stairs_array;    

    for (int i = 0; i < 10; i++)
    {
        if (stairs_array[i].y == 15 && (stairs_array[i].x <= x && stairs_array[i].x + 4 >= x))
        {
            return 1;
        }
    }
    
    return 0;
}


int stairs_checker(struct stairs_data *stairs_data)
{
    struct stairs_data *data = stairs_data;
    struct stairs *stairs_array = data->stairs_array;   

    for (int i = 0; i < 10; i++)
    {
        if (stairs_array[i].y > 0)
        {
            return 0;
        }
    }
    return 1;
}

void print_lose(WINDOW *win) 
{
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 7, 7,  "__     __         ");
    mvwprintw(win, 8, 7,  "\\ \\   / /         ");
    mvwprintw(win, 9, 7,  " \\ \\_/ /__  _   _ ");
    mvwprintw(win, 10, 7, "  \\   / _ \\| | | |");
    mvwprintw(win, 11, 7, " _ | | (_) | |_| |");
    mvwprintw(win, 12, 7, "| ||_|\\___/ \\__,_|");
    mvwprintw(win, 13, 7, "| | ___  ___  ___ ");
    mvwprintw(win, 14, 7, "| |/ _ \\/ __|/ _ \\");
    mvwprintw(win, 15, 7, "| | (_) \\__ \\  __/");
    mvwprintw(win, 16, 7, "|_|\\___/|___/\\___|");

    wrefresh(win);
}

int main_print_2(WINDOW *win)
{
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 1,5, "Due to the lack of");
    mvwprintw(win, 2,1, "experience of the developer,");
    mvwprintw(win, 3,7, "the probability");
    mvwprintw(win, 4,7, "of your victory");
    mvwprintw(win, 5,1, " is also a matter of chance");

    mvwprintw(win, 7, 4,  " _____  _             ");
    mvwprintw(win, 8, 4, "|  __ \\| |            ");
    mvwprintw(win, 9, 4, "| |__) | | __ _ _   _ ");
    mvwprintw(win, 10, 4, "|  ___/| |/ _` | | | |");
    mvwprintw(win, 11, 4, "| |    | | (_| | |_| |");
    mvwprintw(win, 12, 4, "|_|    |_|\\__,_|\\__, |");
    mvwprintw(win, 13, 4, "                 __/ |");
    mvwprintw(win, 14, 4, "                |___/ ");

    mvwprintw(win, 16, 1,  "#_##########################");
    mvwprintw(win, 17, 1, "| |#########################");
    mvwprintw(win, 18, 1, "| |#####___##__#___###_____#");
    mvwprintw(win, 19, 1, "| | ## / _ \\/ _` \\ \\ / / _ \\");
    mvwprintw(win, 20, 1, "| |___|  __/ (_| |\\ V /  __/");
    mvwprintw(win, 21, 1, "|______\\___|\\__,_| \\_/ \\___|");

    wrefresh(win);
}



int main_print(WINDOW *win)
{
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 1,5, "Due to the lack of");
    mvwprintw(win, 2,1, "experience of the developer,");
    mvwprintw(win, 3,7, "the probability");
    mvwprintw(win, 4,7, "of your victory");
    mvwprintw(win, 5,1, " is also a matter of chance");

    mvwprintw(win, 7, 4,  "#_____##_#############");
    mvwprintw(win, 8, 4,  "|  __ \\| |############");
    mvwprintw(win, 9, 4, "| |__) | |#__#_#_###_#");
    mvwprintw(win, 10, 4, "|  ___/| |/ _` | | | |");
    mvwprintw(win, 11, 4, "| |    | | (_| | |_| |");
    mvwprintw(win, 12, 4, "|_|    |_|\\__,_|\\__, |");
    mvwprintw(win, 13, 4, "#################__/ |");
    mvwprintw(win, 14, 4, "################|___/#");

    mvwprintw(win, 16, 1,  " _                          ");
    mvwprintw(win, 17, 1, "| |                         ");
    mvwprintw(win, 18, 1, "| |     ___  __ ___   _____ ");
    mvwprintw(win, 19, 1, "| |    / _ \\/ _` \\ \\ / / _ \\");
    mvwprintw(win, 20, 1, "| |___|  __/ (_| |\\ V /  __/");
    mvwprintw(win, 21, 1, "|______\\___|\\__,_| \\_/ \\___|");

    wrefresh(win);
}
