//Hulea Andrei-Florin , Grupa 30225
//Sisteme de operare
//Tema 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>

//functie pentru printare sub format binar a unui unsigned
void binary_print(unsigned numar, int nr_biti)
{
	unsigned i;
	for (i = 1 << (nr_biti - 1); i > 0; i = i / 2)
		(numar & i) ? printf("1") : printf("0");
}

//functia returneaza frecventa de aparitie a bitilor
//afiseaza eroare pentru parametri gresiti
//MOD APELARE: ./t1 histogram bits=<number_of_bits> path=<file_path>
int *histogram(char *bits, char *path, int printcheck)
{

	int fd;
	if ((fd = open(path, O_RDONLY)) == -1)
	{
		printf("ERROR\nInvalid file path\n");
		exit(3);
	}

	unsigned char byte_read[8];

	int no_of_octets;
	char nume[6];
	if ((sscanf(bits, "%5c %d", nume, &no_of_octets)) == -1)
	{
		printf("An error has occured in sscanf\n");
		exit(4);
	}

	if (no_of_octets != 1 && no_of_octets != 2 && no_of_octets != 4 && no_of_octets != 8)
	{
		printf("ERROR\nInvalid number of bits\nSupported values are: 1,2,4,8 \n");
		exit(2);
	}

	int var;
	int *frecv = (int *)malloc(256 * sizeof(int));
	for (int i = 0; i < 256; i++)
		frecv[i] = 0;

	while ((var = read(fd, &byte_read, no_of_octets)) > 0)
	{
		for (int i = 0; i < no_of_octets; i++)

		{
			if (no_of_octets == 1)
			{
				int j = 0;
				while (j < 8)
				{
					frecv[(int)byte_read[i] & 0x01]++;
					byte_read[i] = byte_read[i] >> 1;

					j++;
				}
			}
			else if (no_of_octets == 2)
			{
				int j = 0;
				while (j < 4)
				{
					frecv[(int)byte_read[i] & 0x03]++;
					byte_read[i] = byte_read[i] >> 2;

					j++;
				}
			}
			else if (no_of_octets == 4)
			{
				int j = 0;
				while (j < 2)
				{
					frecv[(int)byte_read[i] & 0x0F]++;
					byte_read[i] = byte_read[i] >> 4;

					j++;
				}
			}
			else if (no_of_octets == 8)
			{
				frecv[(int)byte_read[i] & 0xFF]++;
			}
		}
	}

	int k;

	if (no_of_octets == 1)
		k = 2;
	if (no_of_octets == 2)
		k = 4;
	if (no_of_octets == 4)
		k = 16;
	if (no_of_octets == 8)
		k = 256;

	if (printcheck == 1)
	{
		printf("SUCCES\n");
		for (int i = 0; i < k; i++)
		{
			binary_print(i, no_of_octets);
			printf(" : ");
			printf("%d \n", frecv[i]);
		}
	}

	return frecv;

	free(frecv);
}

//functia afiseaza cea mai lunga secventa de biti de 1 consecutivi
//cat si offset-ul de la care aceasta incepe
//MOD APELARE:./t1 runspath=<file_path>
void runs(char *path)
{

	int fd;
	if ((fd = open(path, O_RDONLY)) == -1)
	{
		printf("ERROR\nInvalid file path\n");
		exit(3);
	}

	unsigned char byte_read;

	int var;
	int current_sequence = 0;
	int longest_sequence = 0;
	int byte = 0;
	int last_byte = 0;
	int bit = 0;
	int last_bit = 0;

	while ((var = read(fd, &byte_read, 1)) > 0)
	{
		bit = 0;

		int j = 0;
		while (j < 8)
		{
			if ((byte_read & 0x80) == 0x80)
			{
				current_sequence++;

				if (current_sequence > longest_sequence)
				{
					longest_sequence = current_sequence;
					last_byte = byte;
					last_bit = bit;
				}
			}
			else
				current_sequence = 0;

			byte_read = byte_read << 1;
			bit++;
			j++;
		}

		byte++;
	}

	last_bit = last_bit - longest_sequence + 1;

	while (last_bit <= 0)
	{
		last_bit = last_bit + 8;
		last_byte = last_byte - 1;
	}

	printf("SUCCESS\n");
	printf("Length of longest run: %d\n", longest_sequence);
	printf("Offset: %d bytes + %d bits\n", last_byte, last_bit);
}

//functie pentru inversarea unui string
char *string_reverse(char *str)
{
	char *p1, *p2;

	if (!str || !*str)
		return str;

	for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
	{
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}
	return str;
}

//citeste modelele de template din t_path si le verifica in fisierul path
//daca fisierul un este valid afiseaza un mesaj de eroare
//MODURI AFISARE:./t1 templatebits=<number_of_bits>template=<number_of_template>t_path=<template_file_path>path=<file_path>
//./t1 templatebits=<number_of_bits>t_path=<template_file_path>path=<file_path>
int template(char *bits, char *template, char *t_path, char *path)
{

	int nr_of_bits;
	char nume1[5];
	if ((sscanf(bits, "%5c %d", nume1, &nr_of_bits)) == -1)
	{
		printf("An error has occured in sscanf\n");
		exit(4);
	}

	if (nr_of_bits != 8 && nr_of_bits != 16 && nr_of_bits != 24 && nr_of_bits != 32)
	{
		printf("ERROR\nInvalid number of bits\n");
		printf("Supported values are : 8,16,24,32");
		exit(1);
	}

	int nr_of_template;
	char nume2[9];
	if ((sscanf(template, "%9c %d", nume2, &nr_of_template)) == -1)
	{
		printf("An error has occured in sscanf\n");
		exit(4);
	}

	int fd;
	if ((fd = open(t_path, O_RDONLY)) == -1)
	{
		printf("ERROR\nInvalid template file path");
		exit(2);
	}

	int fd2;
	if ((fd2 = open(path, O_RDONLY)) == -1)
	{
		printf("ERROR\nInvalid file path\n");
		exit(3);
	}

	short int versiune;
	read(fd, &versiune, 2);

	if (!(versiune >= 12345 && versiune <= 54321))
	{
		printf("ERROR\nInvalid version\n");
		exit(4);
	}

	short int nr_of_template_headers;
	read(fd, &nr_of_template_headers, 2);

	short int size_of_template[255];
	short int nr_of_templates_from_category[255];
	int offset[255];

	for (int i = 0; i < nr_of_template_headers; i++)
	{
		read(fd, &size_of_template[i], 2);
		read(fd, &nr_of_templates_from_category[i], 2);
		read(fd, &offset[i], 4);
	}

	short int toReturn = 0;

	for (int i = 0; i < nr_of_template_headers; i++)
	{
		if (size_of_template[i] == nr_of_bits)
		{

			toReturn = nr_of_templates_from_category[i];
		}
	}

	int wanted_template = 0;

	for (int i = 0; i < nr_of_template_headers; i++)
	{
		if (size_of_template[i] == nr_of_bits)
		{
			lseek(fd, offset[i], SEEK_SET);
			int file;
			int j = 0;

			while ((j < nr_of_template) && (file = read(fd, &wanted_template, nr_of_bits / 8)))
			{
				j++;
			}
		}
	}

	int alocare = 8 * lseek(fd2, 0, SEEK_END);

	char *toSearch = (char *)malloc(alocare * sizeof(char));

	while (wanted_template > 0)
	{
		if (wanted_template & 1)
			strcat(toSearch, "1");
		else
			strcat(toSearch, "0");

		wanted_template = wanted_template >> 1;
	}

	string_reverse(toSearch);

	int nr_aparitii = 0;
	int file2;

	char *searched_file = (char *)malloc(alocare * sizeof(char));
	unsigned char byte_read;
	int k = 0;
	lseek(fd2, 0, SEEK_SET);

	while ((file2 = read(fd2, &byte_read, 1)) > 0)
	{
		for (int i = 0; i < 8; i++)
		{
			if ((byte_read & (1 << 7)) == 0x80)
			{
				searched_file[k++] = '1';
			}
			else
			{
				searched_file[k++] = '0';
			}
			byte_read = byte_read << 1;
		}
	}

	while ((searched_file = strstr(searched_file, toSearch)) != NULL)
	{
		nr_aparitii++;
		searched_file++;
	}

	printf("Occurences of template %d of length %d : ", nr_of_template, nr_of_bits);
	printf("%d\n", nr_aparitii);

	return toReturn;
	free(searched_file);
	free(toSearch);
}

//functia poate afisa una dintre urmatoarele:
//-numele fisierelor din director/subdirectoare care pot fi de tip template-uri
//-numele fisierelor din director/subdirectoare care pot fi de tip histograma
//-numele fisierelor din director/subdirector care contin string-ul dat ca argument in nume
//-numele fisierelor din director/subdirector care au dimensiunea mai mare decat numarul dat ca argument
//optiunea de parcurgere recursiva este optionala
//MOD APELARE: ./t1 list [recursive] <filtering_options> path=<dir_path>
//ex path: path=/home/nume/Desktop/...(path-ul pana la director)
//Filtering Options:template=ok,hist_random=N,name_contains=string,size_greater=value
void list(int recursive, char *filtering, char *path)
{
	char nume[30];
	int nr_of_bits;
	int size_aux;
	char string_aux[30];

	if ((sscanf(filtering, "%[^=]s", nume)) == -1)
	{
		printf("An error has occured in sscanf\n");
		exit(4);
	}
	if (strcmp(nume, "template") == 0)
	{
		DIR *dir = NULL;
		struct dirent *entry = NULL;
		char full_path[1024];
		struct stat statbuf;

		dir = opendir(path);

		if (dir == NULL)
		{
			printf("Invalid directory path");
			exit(1);
		}

		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
			{
				snprintf(full_path, 1024, "%s/%s", path, entry->d_name);

				if (lstat(full_path, &statbuf) == 0)
				{

					if (S_ISREG(statbuf.st_mode))
					{

						int ok = 1;

						int fd2;
						if ((fd2 = open(entry->d_name, O_RDONLY)) == -1)
						{
							printf("ERROR\nInvalid file path\n");
							ok = 0;
						}

						short int versiune;
						read(fd2, &versiune, 2);

						if (!(versiune >= 12345 && versiune <= 54321))
							ok = 0;

						short int nr_of_template_headers;
						read(fd2, &nr_of_template_headers, 2);

						short int size_of_template[nr_of_template_headers];
						short int nr_of_templates_from_category[nr_of_template_headers];
						int offset[nr_of_template_headers];

						for (int i = 0; i < nr_of_template_headers; i++)
						{
							size_of_template[i] = 0;
							nr_of_templates_from_category[i] = 0;
							offset[i] = 0;
						}

						for (int i = 0; i < nr_of_template_headers; i++)
						{
							if ((read(fd2, &size_of_template[i], 2)) < 0)
								ok = 0;

							if (size_of_template[i] != 8 && size_of_template[i] != 16 && size_of_template[i] != 24 && size_of_template[i] != 32)
								ok = 0;

							if ((read(fd2, &nr_of_templates_from_category[i], 2)) < 0)
								ok = 0;

							if ((read(fd2, &offset[i], 4)) < 0)
								ok = 0;
						}

						if (ok == 1)
						{
							printf("Absolute path: %s\n", full_path);
						}
					}

					else if (S_ISDIR(statbuf.st_mode) && recursive == 1)
					{
						list(recursive, filtering, full_path);
					}
				}
			}
		}

		closedir(dir);
	}
	else if (strcmp(nume, "hist_random") == 0)
	{
		if ((sscanf(filtering, "%12c %d", nume, &nr_of_bits)) == -1)
		{
			printf("An error has occured in sscanf\n");
			exit(5);
		}

		char arg[30];
		sprintf(arg, "bits=%d", nr_of_bits);

		DIR *dir = NULL;
		struct dirent *entry = NULL;
		char full_path[1024];
		struct stat statbuf;

		dir = opendir(path);

		if (dir == NULL)
		{
			printf("Invalid directory path");
			exit(1);
		}

		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
			{
				snprintf(full_path, 1024, "%s/%s", path, entry->d_name);

				if (lstat(full_path, &statbuf) == 0)
				{
					if (S_ISREG(statbuf.st_mode))
					{
						int *hist = histogram(arg, full_path, 0);

						int k;

						if (nr_of_bits == 1)
							k = 2;
						if (nr_of_bits == 2)
							k = 4;
						if (nr_of_bits == 4)
							k = 16;
						if (nr_of_bits == 8)
							k = 256;

						int suma = 0;

						for (int i = 0; i < k; i++)
							suma += hist[i];

						int verif = 1;

						for (int i = 0; i < k; i++)
							for (int j = 0; j < k; j++)
								if ((float)abs(hist[i] - hist[j]) > (1.0 * suma / 100))
									verif = 0;

						if (verif == 1)
							printf("Absolute path: %s\n", full_path);
					}
					else if (S_ISDIR(statbuf.st_mode) && recursive == 1)
					{

						list(recursive, filtering, full_path);
					}
				}
			}
		}
		closedir(dir);
	}
	else if (strcmp(nume, "name_contains") == 0)
	{
		if ((sscanf(filtering, "%14c %s", nume, string_aux)) == -1)
		{
			printf("An error has occured in sscanf\n");
			exit(4);
		}

		DIR *dir = NULL;
		struct dirent *entry = NULL;
		char full_path[1024];
		struct stat statbuf;

		dir = opendir(path);

		if (dir == NULL)
		{
			printf("Invalid directory path");
			exit(1);
		}

		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
			{
				snprintf(full_path, 512, "%s/%s", path, entry->d_name);
				if (lstat(full_path, &statbuf) == 0)
				{
					if (S_ISDIR(statbuf.st_mode) && recursive == 1)
					{
						printf("%s\n", entry->d_name);
						list(recursive, filtering, entry->d_name);
					}
					else if (S_ISREG(statbuf.st_mode))
					{
						if (strstr(entry->d_name, string_aux))
							printf("%s\n", entry->d_name);
					}
				}
			}
		}
		closedir(dir);
	}
	else if (strcmp(nume, "size_greater") == 0)
	{
		if ((sscanf(filtering, "%13c %d", nume, &size_aux)) == -1)
		{
			printf("An error has occured in sscanf\n");
			exit(4);
		}

		DIR *dir = NULL;
		struct dirent *entry = NULL;
		char full_path[1024];
		struct stat statbuf;

		dir = opendir(path);

		if (dir == NULL)
		{
			printf("Invalid directory path");
			exit(1);
		}

		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
			{
				snprintf(full_path, 512, "%s/%s", path, entry->d_name);
				if (lstat(full_path, &statbuf) == 0)
				{
					if (S_ISDIR(statbuf.st_mode) && recursive == 1)
					{
						list(recursive, filtering, full_path);
					}
					else if (S_ISREG(statbuf.st_mode))
					{
						if (statbuf.st_size > size_aux)
							printf("%s\n", full_path);
					}
				}
			}
		}
		closedir(dir);
	}
	else
	{
		printf("Incorrect argument\n");
		exit(1);
	}
}

int main(int argc, char **argv)
{
	if (strcmp(argv[1], "histogram") == 0)
	{
		histogram(argv[2], argv[3] + 5, 1);
	}
	else if (strcmp(argv[1], "runs") == 0)
	{
		runs(argv[2] + 5);
	}
	else if (strcmp(argv[1], "template") == 0)
	{

		if (argc == 6)
		{
			template(argv[2], argv[3], argv[4] + 7, argv[5] + 5);
		}
		else
		{
			char saux[50];
			char numep[50];
			sscanf(argv[4], "%5c %s", numep, saux);
			strcpy(argv[4], saux);

			char saux2[50];
			char numep2[50];
			sscanf(argv[3], "%7c %s", numep2, saux2);
			strcpy(argv[3], saux2);

			char *aux = "template=1";

			int first = 0;
			first = template(argv[2], aux, argv[3], argv[4]);

			int i = 2;
			while (i <= first)
			{
				char ex[50] = "template=";
				char toCat[50];

				sprintf(toCat, "%d", i);

				strcat(ex, toCat);

				template(argv[2], ex, argv[3], argv[4]);

				i++;
			}
		}
	}
	else if (strcmp(argv[1], "list") == 0)
	{
		printf("SUCCESS\n");
		if (strcmp(argv[2], "recursive") != 0)
			list(0, argv[2], argv[3] + 5);
		else
			list(1, argv[3], argv[4] + 5);
	}
	else
		printf("ERROR\nThe specified function does not exist!\n");
}