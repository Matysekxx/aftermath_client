#include "Application.h"

int main() {
    const Application app("ws://localhost:8080");
    app.execute();

    return 0;
}