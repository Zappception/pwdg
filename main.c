#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <unistd.h>


typedef struct
{
	long  lLength;
	bool  boRandomLength;
	long  lMinLength;
	long  lMaxLength;
	bool  boNumeric;
	bool  boAlphabeticalLowerCase;
	bool  boAlphabeticalUpperCase;
	bool  boSpecial;
	bool  boOutfile;
	FILE *pfOutfile;
	bool  boDebug;
} CONFIG_TYPE;

static const char a1cHelp[] = {
	"pwdg - a simple password generator                             \n"
	"default arguments are: -l 16 -a -A -n -s                       \n"
	"-h, --help            Show this message                        \n"
	"-l, --length          Length of the password                   \n"
	"-m, --minlength       Minimum length of the password           \n"
	"-M, --maxlength       Maximum length of the password           \n"
	"-n, --numeric         Enable numeric characters                \n"
	"-a, --alphabetical    Enable lower case alphabetical characters\n"
	"-A, --Alphabetical    Enable upper case alphabetical characters\n"
	"-s, --special         Enable special characters                \n"
	"-o, --outfile         Output file                              \n"
	"-d, --debug           Enable debug output                      \n"
};


#define DEFAULT_PASSWORD_LENGTH 16


void init_config (CONFIG_TYPE *ptConfig)
{
	memset(ptConfig, 0, sizeof(*ptConfig));

	ptConfig->lLength   = DEFAULT_PASSWORD_LENGTH;
	ptConfig->pfOutfile = NULL;
}

void read_command_line_arguments (int argc, char **argv, CONFIG_TYPE *ptConfig)
{
	bool boDefaultCharacterArgs = true;
	int flag;
	static struct option longopts[] = {
			{"help",         no_argument,       NULL, 'h'},
			{"length",       required_argument, NULL, 'l'},
			{"minlength",    required_argument, NULL, 'm'},
			{"maxlength",    required_argument, NULL, 'M'},
			{"numeric",      no_argument,       NULL, 'n'},
			{"alphabetical", no_argument,       NULL, 'a'},
			{"Alphabetical", no_argument,       NULL, 'A'},
			{"special",      no_argument,       NULL, 's'},
			{"outfile",      required_argument, NULL, 'o'},
			{"debug",        no_argument,       NULL, 'd'},
			{NULL,           0,                 NULL, 0}
	};
	char *pc;

	while ( (flag = getopt_long(argc, argv, "hl:m:M:naAso:d", longopts, NULL)) != -1)
	{
		switch (flag)
		{
			case 'h':
			{
				printf(a1cHelp);
				exit(0);
			}
			break;

			case 'l':
			{
				ptConfig->lLength = strtol(optarg, &pc, 10);
			}
			break;

			case 'm':
			{
				ptConfig->lMinLength     = strtol(optarg, &pc, 10);
				ptConfig->boRandomLength = true;
			}
			break;

			case 'M':
			{
				ptConfig->lMaxLength     = strtol(optarg, &pc, 10);
				ptConfig->boRandomLength = true;
			}
			break;

			case 'n':
			{
				ptConfig->boNumeric    = true;
				boDefaultCharacterArgs = false;
			}
			break;

			case 'a':
			{
				ptConfig->boAlphabeticalLowerCase = true;
				boDefaultCharacterArgs            = false;
			}
			break;

			case 'A':
			{
				ptConfig->boAlphabeticalUpperCase = true;
				boDefaultCharacterArgs            = false;
			}
			break;

			case 's':
			{
				ptConfig->boSpecial    = true;
				boDefaultCharacterArgs = false;
			}
			break;

			case 'o':
			{
				ptConfig->boOutfile = true;
				if ((ptConfig->pfOutfile = fopen(optarg, "w")) == NULL)
				{
					fprintf(stderr, "could not open %s\n", optarg);
					exit(1);
				}
			}
			break;

			case 'd':
			{
				ptConfig->boDebug = true;
			}
			break;

			default:
			{
				// ignore
			}
			break;
		}
	}

	if (boDefaultCharacterArgs)
	{
		ptConfig->boNumeric               = true;
		ptConfig->boAlphabeticalLowerCase = true;
		ptConfig->boAlphabeticalUpperCase = true;
		ptConfig->boSpecial               = true;
	}

	if (ptConfig->boDebug)
	{
		printf("lLength:                 %ld\n", ptConfig->lLength);
		printf("boRandomLength:          %s\n",  ptConfig->boRandomLength?"true":"false");
		printf("lMinLength:              %ld\n", ptConfig->lMinLength);
		printf("lMaxLength:              %ld\n", ptConfig->lMaxLength);
		printf("boNumeric:               %s\n",  ptConfig->boNumeric?"true":"false");
		printf("boAlphabeticalLowerCase: %s\n",  ptConfig->boAlphabeticalLowerCase?"true":"false");
		printf("boAlphabeticalUpperCase: %s\n",  ptConfig->boAlphabeticalUpperCase?"true":"false");
		printf("boSpecial:               %s\n",  ptConfig->boSpecial?"true":"false");
		printf("boOutfile:               %s\n",  ptConfig->boOutfile?"true":"false");
		printf("*pfOutfile:              %p\n",  (void*)ptConfig->pfOutfile);
		printf("boDebug:                 %s\n",  ptConfig->boDebug?"true":"false");
	}
}

bool character_is_selected (int c, CONFIG_TYPE* ptConfig)
{
	// ASCII Table
	//   30 40 50 60 70 80 90 100 110 120
	// ---------------------------------
	// 0:    (  2  <  F  P  Z  d   n   x
	// 1:    )  3  =  G  Q  [  e   o   y
	// 2:    *  4  >  H  R  \  f   p   z
	// 3: !  +  5  ?  I  S  ]  g   q   {
	// 4: "  ,  6  @  J  T  ^  h   r   |
	// 5: #  -  7  A  K  U  _  i   s   }
	// 6: $  .  8  B  L  V  `  j   t   ~
	// 7: %  /  9  C  M  W  a  k   u  DEL
	// 8: &  0  :  D  N  X  b  l   v
	// 9: '  1  ;  E  O  Y  c  m   w
	if (ptConfig->boNumeric)
	{
		if (   (c >= 48)
			&& (c <= 57) )
		{
			return true;
		}
	}

	if (ptConfig->boAlphabeticalLowerCase)
	{
		if (   (c >= 97)
			&& (c <= 122) )
		{
			return true;
		}
	}

	if (ptConfig->boAlphabeticalUpperCase)
	{
		if (   (c >= 65)
			&& (c <= 90) )
		{
			return true;
		}
	}

	if (ptConfig->boSpecial)
	{
		if (   (   (c >= 32)
			&& (c <= 47) )
		||     (   (c >= 58)
			&& (c <= 64) )
		||     (   (c >= 91)
			&& (c <= 96) )
		||     (   (c >= 123)
			&& (c <= 126) ) )
		{
			return true;
		}
	}

	return false;
}

char get_random_character(CONFIG_TYPE *ptConfig)
{
	int c = 0;

	// printable ASCII characters: 32(space) - 126(~)
	// diff: 94

	do {
		c = rand() % 94;
		c += 32;
		if (ptConfig->boDebug)
		{
			printf("c: %d\n", c);
		}
	} while (!character_is_selected(c, ptConfig));

	return (char)c;
}

// https://stackoverflow.com/a/323302
unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

int main (int argc, char **argv)
{
	CONFIG_TYPE    tConfig;
	char          *pcPassword;
	unsigned long  seed;

	init_config(&tConfig);

	read_command_line_arguments(argc, argv, &tConfig);

	// https://stackoverflow.com/a/323302
	seed = mix(clock(), time(NULL), getpid());
	srand(seed);

	if (tConfig.boRandomLength)
	{
		tConfig.lLength = (long)(tConfig.lMinLength + (rand() % (tConfig.lMaxLength - tConfig.lMinLength + 1)));
	}

	if ((pcPassword = malloc(sizeof(char) * (tConfig.lLength + 1))) == NULL)
	{
		fprintf(stderr, "could not allocate space for the password\n");
		exit(1);
	}

	for (long i = 0; i < tConfig.lLength; i++)
	{
		pcPassword[i] = get_random_character(&tConfig);
	}
	pcPassword[tConfig.lLength] = '\0';

	if (tConfig.boOutfile)
	{
		fprintf(tConfig.pfOutfile, "%s\n", pcPassword);
		fclose(tConfig.pfOutfile);
	}
	else
	{
		printf("%s\n", pcPassword);
	}

	free(pcPassword);
	return 0;
}
