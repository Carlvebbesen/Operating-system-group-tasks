#define _POSIX_SOURCE
#define _XOPEN_SOURCE
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>

struct alarm_schedule {
        struct tm scheduled_time_struct;
        pid_t process_id;
        int is_active;
};

void playSound()
{
    srand ( time(NULL) );
    int number = rand() % 4;
    #ifdef _WIN32
        printf("You use Windows so you don't get any cool sounds :'(")
    #elif __APPLE__
        switch (number)
        {
        case 0:
            execlp("afplay", "afplay", "./verstappen.mp3", 0);
            break;

        case 1:
            execlp("afplay", "afplay", "./hamilton.mp3", 0);
            break;

        case 2:
            execlp("afplay", "afplay", "./bottas.mp3", 0);
            break;

        case 3:
            execlp("afplay", "afplay", "./sonicboom.mp3", 0);
            break;

        default:
            printf("Something went wrong ;( RING RING");
            break;
  }
    #elif __unix__
        switch (number)
        {
        case 0:
            execlp("mpg123", "mpg123", "-q", "./bottas.mp3", 0);
            break;

        case 1:
            execlp("mpg123", "mpg123", "-q", "./sonicboom.mp3", 0);
            break;

        case 2:
            execlp("mpg123", "mpg123", "-q", "./bottas.mp3", 0);
            break;

        case 3:
            execlp("mpg123", "mpg123", "-q", "./sonicboom.mp3", 0);
            break;

        default:
            printf("Something went wrong ;( RING RING");
            break;
        }
    #endif
}

struct alarm_schedule scheduled_alarms[8];
int next_free_index = 0;
time_t current_time;
pid_t createAlarm(double duration){
        int new_pid;
        new_pid = fork();
        if(new_pid == 0)
        {
         sleep(duration);
         playSound();
         exit(0);
        }
        else
        {
          return new_pid;
        }
}
int get_next_free_index() {
  int i;
  for(i = 0; i < 8; ++i) {
    if (scheduled_alarms[i].is_active == 0) {
      return i;
    }
  }
  return -1;
}

int is_date_valid(struct tm time_struct) {

	if (time_struct.tm_sec > 59) {
		return 0;
	}
	if (time_struct.tm_min > 59) {
		return 0;
	}
	if (time_struct.tm_hour > 23) {
		return 0;
	}
	if (time_struct.tm_mday > 31) {
		return 0;
	}
	
	int mon = time_struct.tm_mon;

	if (mon > 11) {
		return 0;
	}
	if (time_struct.tm_year > 9999 || time_struct.tm_year < 0) {
		return 0;
        }
	if (mon == 1) {
		if (time_struct.tm_year % 4 == 0) {
			if (time_struct.tm_mday > 29) {
				return 0;
			}
		} else {
			if (time_struct.tm_mday > 28) {
				return 0;
			}
		}
	}
	if (mon == 3 || mon == 5 || mon == 8 || mon == 10) {
		if (time_struct.tm_mday > 30) {
			return 0;
		}
	}
	return 1;
}


int main(void)
{
        char date_time[19];
        current_time = time(NULL);
        printf("Welcome to the alarm clock! It is currently %s\n", ctime(&current_time));
        do {
                char input_command;
                printf("Please enter \"s\"  (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n");
                scanf("%[^\n]%*c", &input_command);
                waitpid(-1, NULL, WNOHANG);
                switch(input_command) {
                        case 's':
                                printf("Enter time: \n");
                                scanf("%[^\n]%*c", date_time);

           
	


/*
				regex_t regex;
				int valid;
				valid = regcomp(&regex, "%Y-%m-%d %T", 0);
				printf("Valid: %d\n",valid); 
				valid = regexec(&regex, date_time, 0, NULL, 0);
				printf("Valid: %d\n",valid);
				if (valid != 0) {
   				  printf("Invalid input\n");
				  break;
				}
*/

                                next_free_index = get_next_free_index();
                                if (next_free_index == -1){
                                        printf("No available slots.\n");
                                } else {
                                        struct tm time_struct;
                                        strptime(date_time, "%Y-%m-%d %T", &time_struct);
                                         
					if (is_date_valid(time_struct)) {						

					 time_t time_base;
                                         double time_diff;
                                         char dtsout_buff[70];
                                         current_time = time(NULL);
                                         scheduled_alarms[next_free_index].scheduled_time_struct = time_struct;
                                         scheduled_alarms[next_free_index].is_active = 1;
                                         scheduled_alarms[next_free_index].scheduled_time_struct.tm_isdst = 0;
                                         time_base = mktime(&scheduled_alarms[next_free_index].scheduled_time_struct);
                                         time_diff = difftime(time_base, current_time);
					 if (time_diff < 0) {
					    printf("Please set a time that is not in the past.\n");
					    break;
					 }
                                         scheduled_alarms[next_free_index].process_id = createAlarm(time_diff);
                                         strftime(dtsout_buff, sizeof dtsout_buff, "%Y-%m-%d %T", &scheduled_alarms[next_free_index].scheduled_time_struct);
                                         printf("Pid of new child: %d \nTimediff: %f \n", scheduled_alarms[next_free_index].process_id, time_diff);
                                         printf("Alarm set for %s \n", dtsout_buff);
					 } else {
                                    		printf("Invalid input\n");
                                    
				    	}	
                                }
                                break;
                        case 'l':
                                printf("Scheduled alarms: \n");
                                int i;
                                char dtout_buff[70];
                                for (i = 0; i < 8; ++i){
			          if (scheduled_alarms[i].is_active) {
		             	    scheduled_alarms[i].is_active = difftime(mktime(&scheduled_alarms[i].scheduled_time_struct), time(NULL)) > 0;
				  }
                                  if (scheduled_alarms[i].is_active) {
                                    strftime(dtout_buff, sizeof dtout_buff, "%Y-%m-%d %T", &scheduled_alarms[i].scheduled_time_struct);
                                    printf("Alarm %d at %s \n", i+1, dtout_buff);
                                  }
                                }
                                break;
                        case 'c':
                                printf("Enter index of alarm you want to cancel: \n");
                                char input;
                                int alarm_index;
                                scanf("%[^\n]%*c", &input);
                                alarm_index = atoi(&input) - 1;

				if (scheduled_alarms[alarm_index].is_active) {
                                  scheduled_alarms[alarm_index].is_active = 0;
                                  kill(scheduled_alarms[alarm_index].process_id, SIGKILL);
				} else {
  				  printf("There is no active alarm with the given index\n");
				}                                
				break;
                        case 'x':
                                printf("Exit successfully \n");
                                return 0;
                        default:
                                printf("Invalid command. \n");
                                break;
                }
        } while(1);
}
