#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../watcher/watcher.h"
#include "../imagelib/imagelib.h"
#include "../random/pcg_basic.h"
#include "point.h"
#include "linkedlist.h"
#include "kdtree.h"
#include <math.h>
#include <time.h>

#define MAX(x,y) ((x) >= (y)) ? (x) : (y)
#define MIN(x,y) ((x) <= (y)) ? (x) : (y)

int middle(int a, int b, int c) 						
{   										
	int minVal = MIN(MIN(a, b), c);     			
	int maxVal = MAX(MAX(a, b), c);			 
	return a + b + c - maxVal - minVal;	    
} 


void qselect(Point *a, Point *v, int len, int k, int axis)
{



/* qselect implementation copied from https://rosettacode.org/wiki/Quickselect_algorithm#C
 * and modified heavily with some optimizations for use with the kdtree
 */

#define SWAP(a, b)       \
	{                    \
		if (a != b)      \
		{                \
			tmp = v[a];  \
			v[a] = v[b]; \
			v[b] = tmp;  \
		}                \
	}                    

	int i, st;
	Point tmp;

	/* we select the middle element as our pivot, this increases perfomance as we expect the 
	 * array to become more ordered as we progress, as discussed in github issue
	 */

	SWAP(len - 1, (len - 1)/2);

	for (st = i = 0; i < len - 1; i++)
	{
		if (axis == 0)
		{
			if (v[i].X > v[len - 1].X)
				continue;
		}
		else if (axis == 1)
		{
			if (v[i].Y > v[len - 1].Y)
				continue;
		}
		SWAP(i, st);
		st++;
	}

	SWAP(len - 1, st);

	if (len <= 2)
		return;
	if (k > st)
		qselect(a, v + st, len - st, k - st, axis);
	else if (k < st)
		qselect(a, v, st, k, axis);
}


void split_space(Point *v, Kdtree *kdtree, int lvalue, int rvalue, int axis, int kdbox_size)
{
	/* iterative method to split the space creating boxes, general idea is the following:
	* Step: 1
	*/
	int len = (1 + rvalue - lvalue); /* len is the number of items 1,2,....,N */
	int pivot = lvalue + (len / 2); /* pivot is the position of the mediam */
	/* order the array and put the pivot on the correct position */
	qselect(v, v + lvalue, len, pivot - lvalue, axis);
	/* we create the two boxes that split the space based on our pivot
	 * lbox will contain [lvalue, pivot - 1] and rbox will contain [pivot, rvalue]
	 */
	kdtree_insert(kdtree, axis, v[pivot], lvalue, rvalue, pivot);
	/* if we have more Points left that what we want to put in each box
	 * we split the space in two again 
	 */
	if (len / 2 > kdbox_size)
	{
		split_space(v, kdtree, lvalue, pivot - 1, axis ^ 1, kdbox_size);
		split_space(v, kdtree, pivot, rvalue, axis ^ 1, kdbox_size);
	}
}

int main(int argc, char **argv)
{

	/**************************************************************************/
	/*                              Preparación                               */
	/**************************************************************************/

	if (argc != 4)
	{
		printf("Modo de uso: %s <img.txt> <núcleos> <seed>\nDonde\n", argv[0]);
		printf("\tscene.txt es el archivo donde se describe la escena\n");
		printf("\tnúcleos es la cantidad de núcleos\n");
		printf("\tseed es la semilla aleatoria\n");

		return 1;
	}

	/* Leer la cantidad de núcleos */
	int nuclei_count = atoi(argv[2]);
	if (nuclei_count <= 0)
	{
		printf("Debe haber al menos 1 núcleo\n");
		return 1;
	}

	/* Seteamos la semilla aleatoria*/
	int seed = atoi(argv[3]);
	random_seed(seed);

	/* Lee el archivo de la imagen */
	Image *img = img_png_read_from_file(argv[1]);
	/* Abre la ventana de las dimensiones especificadas */
	watcher_open(img -> height, img -> width);

	// Pinta la imagen en la ventana. Para cada pixel:
	for(int row = 0; row < img -> height; row++)
	{
		for(int col = 0; col < img -> width; col++)
		{
			sleep(0.01);
			// Toma el color del pixel 
			Color c = img -> pixels[row][col];
			// Le dice a la ventana que se ponga ese color 
			watcher_set_color(c.R, c.G,  c.B);
			// Y que pinte específicamente ese pixel
			watcher_paint_pixel(row, col);
		}
	}
	

	/**************************************************************************/
	/*                                  PASO 1                                */
	/*                                                                        */
	/* Crear el conjunto de núcleos de forma aleatoria                        */
	/**************************************************************************/

	Point *nuclei = malloc(nuclei_count * sizeof(Point));

	for (int i = 0; i < nuclei_count; i++)
	{
		nuclei[i].X = random_bounded_double(img->width);
		nuclei[i].Y = random_bounded_double(img->height);
	}

	/**************************************************************************/
	/*                                  PASO 2                                */
	/*                                                                        */
	/* Buscar el núcleo más cercano a cada píxel y asociarlo a él             */
	/**************************************************************************/

	/* OJO: Sólo importa medir la mejora en tiempo de esta sección */

	/* Los elementos de la celda de voronoi asociada a cada núcleo */
	/* Es un arreglo de listas */
	List **cells = calloc(nuclei_count, sizeof(List *));

	times_called = 0;
	int points_per_box = 20;
	Kdtree *kdtree = kdtree_init(img->width, img->height, 2);

	// Calculate the time taken by fun()
    clock_t t;
    t = clock();
	split_space(nuclei, kdtree, 0, nuclei_count - 1, 0, points_per_box);
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
    printf("fun() took %f seconds to execute \n", time_taken);

	// Para cada píxel de la imagen
	
	for (int row = 0; row < img->height; row++)
	{
		for (int col = 0; col < img->width; col++)
		{
			// Identifica cual es el núcleo más cercano al pixel
			int closest_point = kdtree_nns(row, col, kdtree, nuclei);
			
			// Se asocia el píxel a su núcleo más cercano
			cells[closest_point] = list_prepend(cells[closest_point], row, col);
		}
	}
	
	kdtree_destroy(kdtree);
	
	// Para cada píxel de la imagen
	
	/*
	for (int row = 0; row < img->height; row++)
	{
		for (int col = 0; col < img->width; col++)
		{
			// Identifica cual es el núcleo más cercano al pixel
			double closest_distance = INFINITY;
			int closest_point;

			// Se revisa la distancia del pixel con cada núcleo
			for (int i = 0; i < nuclei_count; i++)
			{
				// Guardando siempre el más cercano
				double distance = euclidean_distance(nuclei[i], row, col);
				if (distance < closest_distance)
				{
					closest_distance = distance;
					closest_point = i;
				}
			}
			// Se asocia el píxel a su núcleo más cercano
			cells[closest_point] = list_prepend(cells[closest_point], row, col);
		}
	}
	*/
	

	/**************************************************************************/
	/*                                  PASO 3                                */
	/*                                                                        */
	/* Pintar el diagrama de voronoi de acuerdo a los pixeles de una celda    */
	/**************************************************************************/

	/* Para cada núcleo */
	for (int i = 0; i < nuclei_count; i++)
	{
		/* Nota: los colores se dividen en componentes R,G y B */
		double R = 0;
		double G = 0;
		double B = 0;
		int count = 0;

		/* c es el promedio de los colores de cada los pı́xel dentro de su celda */
		for (List *curr = cells[i]; curr; curr = curr->next)
		{
			Color c = img->pixels[curr->row][curr->col];

			R += c.R;
			G += c.G;
			B += c.B;

			count++;
		}

		R /= count;
		G /= count;
		B /= count;

		/* Pintar de color c todos los píxeles asociados al núcleo */
		watcher_set_color(R, G, B);

		/* Por cada píxel dentro de la celda correspondiente al i-ésimo núcleo */
		for (List *curr = cells[i]; curr; curr = curr->next)
		{
			watcher_paint_pixel(curr->row, curr->col);
		}
		// sleep(0.3);
	}

	/* Imprime la ventana en una imagen para se la muestres a tu mamá */
	// watcher_snapshot("mira_mama_que_lindo_mi_programa.png");
	printf("Number of Euclidian Distance: %lld\n", times_called);
	printf("Pixels: %d\n",img->width * img->height);
	/* Detiene el programa por 5 segundos para que contemples el resultado */
	sleep(4);

	/**************************************************************************/
	/*                          Liberación de Memoria                         */
	/**************************************************************************/

	for (int i = 0; i < nuclei_count; i++)
	{
		list_destroy(cells[i]);
	}
	free(cells);
	free(nuclei);
	img_png_destroy(img);

	/* OJO: Lo que sea que haya en la ventana cuando llames esta función será */
	/* lo que se considere para tu corrección */
	watcher_close();

	/* OJO: Si no retornas 0 en el main, se considera que tu programa falló */
	return 0;
}
