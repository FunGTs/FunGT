#include <iostream>

void paypal(int amount)
{
    std::cout << "Procesando pago con paypal $" << amount << std::endl;
}

void amex(int amount)
{
    std::cout << "Procesando pago con amex $" << amount << std::endl;
}

void clip(int amount)
{
    std::cout << "Procesando pago con clip $" << amount << std::endl;
}

int main()
{
    int choice, amount;

    std::cout << "Amount: ";
    std::cin >> amount;
    std::cout << "Choose payment method: 1-Paypal, 2-Amex, 3-Clip: ";
    std::cin >> choice;

    //////////////////////////////

    // definir un apuntador a funcion

    void (*pointerToPayment)(int) = nullptr;

    switch (choice)
    {
    case 1:
        pointerToPayment = &paypal;
        break;
    case 2:
        pointerToPayment = &amex;
        break;
    case 3:
        pointerToPayment = &clip;
        break;
    default:
        std::cout << "Invalid choice" << std::endl;
        return 1;
    }

     // Aqui ejecutas el pago
    if(pointerToPayment){
         pointerToPayment(amount);
    }
    else{
        std::cout<<"Invalid pointer"<<std::endl;
    }
   

    return 0;
}
