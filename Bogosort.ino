const byte LEN = 7;
int arr[LEN];

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));

  for (byte i = 0; i < LEN; i++) {
    arr[i] = random(1, 100001);
  }

  Serial.print("Initial array: ");
  printArray(arr, LEN);
  Serial.println("Starting bogosort...\n");

  unsigned long attempts = 0;

  while (!isSorted(arr, LEN)) {
    shuffle(arr, LEN);
    attempts++;

    Serial.print("Try ");
    Serial.print(attempts);
    Serial.print(": ");
    printArray(arr, LEN);
    //delay(50);
  }

  Serial.println();
  Serial.print("Sorted after ");
  Serial.print(attempts);
  Serial.println(" tries.");
  Serial.print("Final sorted array: ");
  printArray(arr, LEN);
}


void printArray(int a[], byte len) {
  Serial.print("[");
  for (byte i = 0; i < len; i++) {
    Serial.print(a[i]);
    if (i < len - 1) {
      Serial.print(", ");
    }
  }
  Serial.println("]");
}


bool isSorted(int a[], byte len) {
  for (byte i = 0; i < len - 1; i++) {
    if (a[i] > a[i + 1]) {
      return false;
    }
  }
  return true;
}


void shuffle(int a[], byte len) {
  for (int i = len - 1; i > 0; i--) {
    int j = random(0, i + 1); 
    int tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
  }
}



void loop() {
  
}
