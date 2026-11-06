#include <iostream>
#include <ctime>
#include "CinemaSim.h"
 
 
int main(){
 
    srand(time(0));   // seed, called once
 
    // why front and back?
    // front = where people leave the line (dequeue)
    // back  = where people join the line (enqueue), so we never have to walk the whole list
 
    qNode<Person>* front = nullptr;
    qNode<Person>* back = nullptr;
 
    //==================================
    // PART 1: small test, 5 people
    //==================================
    std::cout << "PART 1: 5 people \n";
 
    Person* few = generatePeople(5);
    mergeSort(few, 5);
 
    // enqueue one by one, and show the front and back every time
    for(int i = 0; i < 5; i++){
        enqueue(&front, &back, few[i]);
        std::cout << front->data << " " << back->data << std::endl;
    }
 
    dequeue(&front, &back);
    std::cout << front->data << " " << back->data << std::endl;
 
    // a new person joins the back of the line
    Person late = {6, 300};
    enqueue(&front, &back, late);
    std::cout << front->data << " " << back->data << std::endl;
 
    std::cout << "Queue list \n";
    display(front);
    std::cout << "Front value: " << FrontVariable(front) << std::endl;
 
    clearQueue(&front, &back);
    std::cout << "After clearQueue: ";
    display(front);
    delete[] few;
 
 
    //==================================
    // PART 2: 1000 people
    //==================================
    std::cout << "\nPART 2: 1000 people \n";
 
    Person* people = generatePeople(1000);
    mergeSort(people, 1000);
    enqueueAll(&front, &back, people, 1000);
 
    std::cout << "Front (poorest): " << front->data << std::endl;
    std::cout << "Back (richest): " << back->data << std::endl;
 
    // count how many people are in the queue
    int count = 0;
    qNode<Person>* current = front;
    while(current != nullptr){
        count++;
        current = current->next;
    }
    std::cout << "People in queue: " << count << std::endl;
 
    // dequeue the first 3 people
    for(int i = 0; i < 3; i++){
        std::cout << "Serving: " << FrontVariable(front) << std::endl;
        dequeue(&front, &back);
    }
    std::cout << "New front: " << FrontVariable(front) << std::endl;
 
    // dequeue everyone else
    int served = 3;
    while(!isEmpty(front)){
        dequeue(&front, &back);
        served++;
    }
    std::cout << "Total served: " << served << std::endl;
 
    std::cout << "Queue list \n";
    display(front);
 
    delete[] people;
 
    return 0;
}
 