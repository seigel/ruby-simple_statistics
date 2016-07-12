#ifndef _SURVIVAL_STAT_UTILITY_H_
#define _SURVIVAL_STAT_UTILITy_H_

#include <stdio.h>
#include <stdlib.h>
#include "global_utility.h"
#include "survival_def.h"
#include "type_def.h"

struct array create_sorted_unique_array(double* array, int size)
{
	struct array arr;
	int i;
	/* For performance, we have not used memset here. This might be risky */
	arr.D_ptr = NULL;
	int count;

	qsort(array, size, sizeof(double), &compare_double);

	count = 1;
	//calcualte number of unique
	for(i = 1; i < size; ++i)
	{
		if(array[i] != array[i-1])
		{
			count++;
		}
	}

	double * unique_arr = (double *) malloc(count * sizeof(double));

	//assign unique elements
	count = 1;
	unique_arr[0] = array[0];
	for(i = 1; i < size; ++i)
	{
		if(array[i] != array[i-1])
		{
			unique_arr[count] = array[i];
			count++;
		}
	}

	arr.D_ptr = unique_arr;
	arr.size = count;

	return arr;
}

array merge_two_array(double* array_1, int size_1, double* array_2, int size_2)
{
	int i;
	struct array arr;

	//To speed up, choose to do not memset arr.
	arr.D_ptr = NULL;

	int total_size = size_1 + size_2;
	double * merged_array = (double *) malloc(total_size * sizeof(double));
	for(i = 0; i < size_1; i++)
	{
		merged_array[i] = array_1[i];
	}

	for( i = 0; i < size_2; i++)
	{
		merged_array[i + size_1] = array_2[i];
	}

	arr.D_ptr = merged_array;
	arr.size = total_size;

	return arr;
}

int find_first_index_has(double* arr, int size, double value)
{
	int i;
	for(i = 0; i < size; i++)
	{
		if(fabs(arr[i] - value) < EPSILON)
		{
			return i;
		}
	}

	//no value in the array
	return -1;
}

struct Group_N group_N_self_range(double* time, int* censored, int size)
{
	int i, count_at, uncensored_num_at, censored_num_at;
	double tmp, time_at;

	// sort time and censored based on time together, time can censored array
	struct point* time_censored_array = (struct point*) malloc(size * sizeof(struct point));

	//censored, if censored[] is positive
	for (i = 0; i < size; i++)
	{
		time_censored_array[i].x = time[i];
		if (censored[i] > 0)
			time_censored_array[i].y = 1;
		else
			time_censored_array[i].y = -1;
	}

	qsort(time_censored_array, size, sizeof(struct point), &point_compare_x);

	//count unique uncensored time point
	int count = 0;
	for (i = 0; i < size; i++)
	{	//uncensored
		if (time_censored_array[i].y < 0)
		{
			if (count == 0)
			{
				count++;
				tmp = time_censored_array[i].x;
			}

			if (count > 0)
			{	//unique
				if (time_censored_array[i].x != tmp)
				{
					count++;
					tmp = time_censored_array[i].x;
				}
			}
		}
	}

	double* unique_uncensored_time = (double *) malloc(count * sizeof(double));

	count = 0;

	for (i = 0; i < size; i++)
	{
		if (time_censored_array[i].y < 0)
		{
			if (count == 0)
			{
				count++;
				unique_uncensored_time[count] = time_censored_array[i].x;
				tmp = time_censored_array[i].x;

			}

			if (count > 0)
			{
				if (time_censored_array[i].x != tmp)
				{
					unique_uncensored_time[count] = time_censored_array[i].x;

					count++;
					tmp = time_censored_array[i].x;
				}
			}
		}
	}

	int* uncensored_num = (int *) malloc(count * sizeof(int));
	int* censored_num = (int *) malloc(count * sizeof(int));

	//record current time point
	time_at = unique_uncensored_time[0];
	count_at = 0;
	uncensored_num_at = 0;
	censored_num_at = 0;

	for (i = 0; i < size; i++)
	{
		if (time_censored_array[i].x <= time_at)
		{
			if (time_censored_array[i].y > 0)
				censored_num_at++;
			else
				uncensored_num_at++;

			if (i == size - 1)
			{
				uncensored_num[count_at] = uncensored_num_at;
				censored_num[count_at] = censored_num_at;
			}

		} else {
			uncensored_num[count_at] = uncensored_num_at;
			censored_num[count_at] = censored_num_at;
			count_at++;

			uncensored_num_at = 0;
			censored_num_at = 0;
			time_at = unique_uncensored_time[count_at];
			
			if (time_censored_array[i].y > 0)
				censored_num_at++;
			else
				uncensored_num_at++;
		}
	}

	Group_N at_risk_result;
	at_risk_result.uncensored = uncensored_num;
	at_risk_result.censored = censored_num;
	at_risk_result.size = count;
	at_risk_result.time = unique_uncensored_time;
	free(time_censored_array);
	return at_risk_result;
}

struct Group_N group_N_given_range(double* time, int* censored, int size, double* unique_time, int unique_time_size)
{
	int i, count_at, uncensored_num_at, censored_num_at;
	double time_at;

	struct point* time_censored_array = (struct point*) malloc(size * sizeof(struct point));

	for (i = 0; i < size; i++)
	{
		time_censored_array[i].x = time[i];
		//not very fast here, prefer to define another point
		if (censored[i] > 0)
			time_censored_array[i].y = 1;
		else
			time_censored_array[i].y = -1;
	}

	qsort(time_censored_array, size, sizeof(struct point), &point_compare_x);

	int* uncensored_num = (int *) malloc(unique_time_size * sizeof(int));
	int* censored_num = (int *) malloc(unique_time_size * sizeof(int));

	for (i = 0; i < unique_time_size; i++)
	{
		uncensored_num[i] = 0;
		censored_num[i] = 0;
	}

	//record current time point
	time_at = unique_time[0];
	count_at = 0;
	uncensored_num_at = 0;
	censored_num_at = 0;

	for (i = 0; i < size; i++)
	{
		if (time_censored_array[i].x <= time_at)
		{
			if (time_censored_array[i].y > 0)
				censored_num_at++;
			else
				uncensored_num_at++;

			if (i == size - 1)
			{
				uncensored_num[count_at] = uncensored_num_at;
				censored_num[count_at] = censored_num_at;
			}

		} else {

			while (time_censored_array[i].x > time_at)
			{
				uncensored_num[count_at] = uncensored_num_at;
				censored_num[count_at] = censored_num_at;
				count_at++;

				uncensored_num_at = 0;
				censored_num_at = 0;
				time_at = unique_time[count_at];
			}

			if (time_censored_array[i].y > 0)
				censored_num_at++;
			else
				uncensored_num_at++;
		}
	}

	Group_N at_risk_result;
	at_risk_result.uncensored = uncensored_num;
	at_risk_result.censored = censored_num;
	at_risk_result.size = unique_time_size;
	at_risk_result.time = unique_time;

	free(time_censored_array);

	return at_risk_result;
}

#endif