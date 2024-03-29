// ConsoleApplication8.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include<Windows.h>

using namespace std;

struct mem_control_bloc
{
	int is_avalible = 1;
	unsigned int size = 0;
	mem_control_bloc*next = 0;
	mem_control_bloc*prev = 0;
	char* p = 0;

};

class SmallAllocator {
private:
	char Memory[104857];
	unsigned int offset = 0;
	struct mem_control_bloc*head = 0;

public:
	SmallAllocator()
	{

		memset(Memory, 0, 104857 * sizeof(char));

	}
	void defrag()
	{
		if (head != 0)
		{
			struct mem_control_bloc* temp = head;
			while (temp->next != 0)
			{
				if (temp->is_avalible)
				{
					if (temp->next->is_avalible)
					{
						struct mem_control_bloc*x = temp->next;
						temp->size += temp->next->size;
						temp->next = temp->next->next;
						x->next->prev = temp;
					}
				}
				temp = temp->next;
			}
		}
	}
	
	void *Alloc(unsigned int Size) 
	{
		defrag();
		void*allocated_mem = 0;
		struct mem_control_bloc memc;
		Size = Size + sizeof(mem_control_bloc);

		if (head == 0)
		{
			memc.size = Size;
			memc.is_avalible = 0;
			memc.p = Memory;
			char*p = Memory;
			memcpy(p,&memc,sizeof(mem_control_bloc));
			offset += Size;
			head = (mem_control_bloc*)p;
			allocated_mem = (void*)((char*)p + sizeof(mem_control_bloc));
			return allocated_mem;
		}
		else
		{
			struct mem_control_bloc*temp = head;
			
			while (temp->next != 0)
			{
				if (temp->is_avalible)
				{
					if (temp->size >= Size)
					{
						
						temp->is_avalible = 0;
						allocated_mem = (void*)((char*)temp->p + sizeof(mem_control_bloc));
						return allocated_mem;
					}
				}
				temp = temp->next;

			}

			memc.size = Size;
			memc.is_avalible = 0;
			memc.p = Memory + offset + 1;
			char*p = Memory+offset+1;
			memcpy(p, &memc, sizeof(mem_control_bloc));
			offset += Size;
			temp->next = (mem_control_bloc*)p;
			temp->next->prev = temp;
			allocated_mem = (void*)((char*)p + sizeof(mem_control_bloc));
			return allocated_mem;

		}
		

	
	};
	void *ReAlloc(void *Pointer, unsigned int Size) 
	{
		defrag();
		Pointer = (char*)Pointer - sizeof(mem_control_bloc);
		int t_size = ((mem_control_bloc*)Pointer)->size-sizeof(mem_control_bloc);
	
		if (((mem_control_bloc*)Pointer)->size < Size)
		{
			void*new_memory = this->Alloc(Size);
			Pointer = (char*)Pointer + sizeof(mem_control_bloc);
			memcpy(new_memory, Pointer, t_size);
			this->Free(Pointer);
			return new_memory;

		}
		else
		{
			Pointer = (char*)Pointer + sizeof(mem_control_bloc);
			return Pointer;
		}
	};
	void Free(void *Pointer) 
	{
		defrag();
		Pointer = (char*)Pointer - sizeof(mem_control_bloc);
		((mem_control_bloc*)Pointer)->is_avalible = 1;
	};
};
int main()
{
	SmallAllocator A1;
	int * A1_P1 = (int *)A1.Alloc(sizeof(int));
	A1_P1 = (int *)A1.ReAlloc(A1_P1, 2 * sizeof(int));
	A1.Free(A1_P1);
	
	SmallAllocator A2;
	int * A2_P1 = (int *)A2.Alloc(10 * sizeof(int));
	for (unsigned int i = 0; i < 10; i++) A2_P1[i] = i;
	for (unsigned int i = 0; i < 10; i++) if (A2_P1[i] != i) std::cout << "ERROR 1" << std::endl;
	int * A2_P2 = (int *)A2.Alloc(10 * sizeof(int));
	for (unsigned int i = 0; i < 10; i++) A2_P2[i] = -1;
	for (unsigned int i = 0; i < 10; i++) if (A2_P1[i] != i) std::cout << "ERROR 2" << std::endl;
	for (unsigned int i = 0; i < 10; i++) if (A2_P2[i] != -1) std::cout << "ERROR 3" << std::endl;
	A2_P1 = (int *)A2.ReAlloc(A2_P1, 20 * sizeof(int));
	for (unsigned int i = 10; i < 20; i++) A2_P1[i] = i;
	for (unsigned int i = 0; i < 20; i++) if (A2_P1[i] != i) std::cout << "ERROR 4" << std::endl;
	for (unsigned int i = 0; i < 10; i++) if (A2_P2[i] != -1) std::cout << "ERROR 5" << std::endl;
	A2_P1 = (int *)A2.ReAlloc(A2_P1, 5 * sizeof(int));
	for (unsigned int i = 0; i < 5; i++) if (A2_P1[i] != i) std::cout << "ERROR 6" << std::endl;
	for (unsigned int i = 0; i < 10; i++) if (A2_P2[i] != -1) std::cout << "ERROR 7" << std::endl;
	A2.Free(A2_P1);
	A2.Free(A2_P2);
    return 0;
}

