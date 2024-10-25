int vroom() {
    return 2;
}

void main() {
    int sum1 = Add(1, 2);
    double sum2 = Add(1.5, 2.5);
    int sum3 = Add(1, 2, 3);

    auto a = Add(2, 3);

    Print("Sum1: " + sum1 + ", Sum2: " + sum2 + ", Sum3: " + sum3);
    Print(""+vroom());
}
