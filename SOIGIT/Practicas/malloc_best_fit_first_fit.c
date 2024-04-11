#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "struct.h"

p_meta_data first_element = NULL;
p_meta_data last_element  = NULL;

#define ALIGN8(x) (((((x)-1)>>3)<<3)+8)
#define MAGIC     0x12345678

/*
p_meta_data search_available_space(size_t size_bytes) {
    p_meta_data current = first_element;

    while (current && !(current->available && current->size_bytes >= size_bytes)) 
        current = current->next;

    return current;
}
*/

p_meta_data search_available_space(size_t size_bytes) {

    p_meta_data current = first_element;
    p_meta_data most_accurate;

    if (!current)
        return current;

    while (current && !(current->available && current->size_bytes >= size_bytes)) 
        current = current->next;

    most_accurate = current;    
    
    if (current)
        current = current->next;

    while (current) { // Miramos todos los bloques de nuestra linked list.

        if (current->available &&                                  // Comprovamos que sea un bloque disponible
           (current->size_bytes < most_accurate->size_bytes) &&    // Comprovamos que el tamaño sea menor al actual
           (current->size_bytes >= size_bytes))                    // Comprovamos que sea mayor o igual a lo pedido 
            
           most_accurate = current;

        current = current->next;
    }

    return most_accurate;
}

p_meta_data request_space(size_t size_bytes) 
{
    p_meta_data meta_data;

    meta_data = (void *) sbrk(0);

    if (sbrk(SIZE_META_DATA + size_bytes) == (void *) -1)
        return (NULL);

    meta_data->size_bytes = size_bytes;
    meta_data->available = 0;
    meta_data->magic = MAGIC;
    meta_data->next = NULL;
    meta_data->previous = NULL;

    return meta_data;
}

void free(void *ptr){

    fprintf(stderr, "Using Free ! \n");
    if (ptr == NULL) {
        fprintf(stderr, "Free pointer NULL\n");    
        return;
    }

    p_meta_data bloque = ((p_meta_data)(ptr - SIZE_META_DATA));

    if (bloque->magic != MAGIC) {
        fprintf(stderr, "Ha habido algun error liberando memoria.");
        return;    
    }

    // Buscamos los bloques contiguos que esten liberados

    // Comprovamos la disponibilidad del bloque de la anterior

    p_meta_data aux = bloque->previous;


    if (aux && aux->available && (aux->magic == MAGIC)) {
        aux->next = bloque->next;

        if (bloque->next)
            bloque->next->previous = aux;

        aux->size_bytes = bloque->size_bytes + aux->size_bytes + SIZE_META_DATA;
        
        bloque = aux;
    }
        
    aux = bloque->next; // Cambiamos el auxiliar al siguiente bloque
    
    // Comprovamos si el bloque esta libre, en caso de estarlo lo juntamos.  
    if (aux && aux->available && (aux->magic == MAGIC)) {
        bloque->next = aux->next;

        if (aux == last_element)
            last_element = bloque;

        if (aux->next)
            aux->next->previous = bloque;

        bloque->size_bytes = bloque->size_bytes + aux->size_bytes + SIZE_META_DATA;
    }
           
    
    // Si todo ha funcionado correctamente marcamos el espacio como libre y retornamos la memoria
    bloque->available = 1;

    fprintf(stderr, "Bytes liberados %lu \n", bloque->size_bytes);
}

void *malloc(size_t size_bytes){

    fprintf(stderr, "My Malloc\n");       

    void *p;
    p_meta_data meta_data;

    if (size_bytes <= 0){
        return NULL;
    }

    // We allocate a size of bytes multiple of 8
    size_bytes = ALIGN8(size_bytes);
    fprintf(stderr, "Malloc %zu bytes\n", size_bytes);

    meta_data = search_available_space(size_bytes);

    if (meta_data){ // free block found
        meta_data->available = 0;
    }
    else{ // no free block found
        meta_data = request_space(size_bytes);
        if (!meta_data)
            return (NULL);

        if (last_element)
            last_element->next = meta_data;
        meta_data->previous = last_element;
        last_element = meta_data;

        if (first_element == NULL) // Is this the first element ?
            first_element = meta_data;
    }

    meta_data->magic = MAGIC;

    p = (void *)meta_data;

    // We return the user a pointer to the space
    // that can be used to store data

    return (p + SIZE_META_DATA);
}

/*
void *calloc(size_t nelem, size_t elsize){

    fprintf(stderr, "CALLOC  \n");

    if (elsize <= 0) return NULL;

    p_meta_data meta_data;

    elsize = ALIGN8(elsize);

    size_t all_size = (nelem * elsize) + (nelem * SIZE_META_DATA);

    // all_size = ALIGN8(all_size);

    meta_data = search_available_space(all_size - SIZE_META_DATA);

    if (!meta_data){
        // Le restamos un SIZE_METADATA porque request_space ya le añade un meta_data.
        meta_data = request_space(all_size-SIZE_META_DATA);

        if (!meta_data)
            fprintf(stderr, "No hay espacio en memoria para reservar.");
    }
    // Devolveremos un puntero a la primera posicion de memoria reservada
    const void* p = (void *) meta_data + SIZE_META_DATA;
    fprintf(stderr, "Estando en CALLOC p: %d \n", (unsigned int) p);

    int iter = nelem;

    fprintf(stderr, "Numero de memsets %d \n", nelem);

    // Para cada bloque de elsize
    for (iter = nelem; iter > 0; iter--){

        if (first_element == NULL) // Si no existeix un lastElement l'asignem com metadata
            first_element = meta_data;
         
        // Lo vinculamos a la lista
        else last_element->next = meta_data;
               
        meta_data->previous = last_element;
        last_element = meta_data;
        
        //memset((void *) meta_data + SIZE_META_DATA, 0, elsize); // Hacemos el memset a 0 de la memoria reservada

        meta_data->size_bytes = elsize;
        meta_data->magic = MAGIC;
        meta_data->available = 0;

        fprintf(stderr, "Se ejecuta el MEMSET size : %d \n", meta_data->size_bytes);

        // meta_data = (p_meta_data) meta_data + meta_data->size_bytes + SIZE_META_DATA;

        meta_data = (p_meta_data) ((void *) meta_data) + SIZE_META_DATA + meta_data->size_bytes;
    }

    return p;

}
*/


void *calloc(size_t nelem, size_t elsize){

    fprintf(stderr, "CALLOC  \n"); // Informamos que se ha entrado en el metodo calloc

    if (elsize <= 0) return NULL;

    elsize = ALIGN8(elsize);

    size_t all_size = (nelem * elsize); 

    void * ptr = malloc(all_size);
    
    memset(ptr, 0, (nelem * elsize));

    return ptr;
}

void *realloc(void * ptr, size_t size_bytes){

    fprintf(stderr, "Realloc de %lu \n", size_bytes);

    if (ptr == NULL) 
        return malloc(size_bytes);

    size_bytes = ALIGN8(size_bytes);

    p_meta_data meta_data = ptr - SIZE_META_DATA; // Conseguimos el bloque de metadata

    if (meta_data->size_bytes >= size_bytes) // Caso b. Hay espacio suficiente en el bloque{
        {
            
        return ptr;
    }

    // Si llega hasta aqui es porque no hay espacio en el bloque, por lo que tenemos que buscar un bloque
    // donde quepa size_bytes y devolver el puntero a ese bloque.

    void * new_ptr = malloc(size_bytes); // Reservamos memoria en otro bloque.

    // Copiamos la memoria que habia en el antiguo bloque en el nuevo bloque
    new_ptr = memcpy(new_ptr, ptr, meta_data->size_bytes);

    // Ahora liberamos la memoria del bloque de origen
    free(ptr);
    
    return new_ptr; // Retornamos el bloque con la nueva memoria.
}