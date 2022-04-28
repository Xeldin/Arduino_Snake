/*                     Codice creato prendendo spunto dall'esempio a questo indirizzo: http://linksprite.com/wiki/index.php5?title=LED_Matrix_Kit Visualizza un carattere alfanumerico sul display a LED 8x8                 */

//================================================

  int Max7219_pinCLK = 10; // pin su arduino connesso al CLK di MAX7219
  int Max7219_pinCS = 9;   // pin su arduino connesso al CAS di MAX7219
  int Max7219_pinDIN = 8;  // pin su arduino connesso al DIN di MAX7219

//=================================================[       COSTANTI       ]=======================================================

  const int buttonPin1 = 2;     // the number of the pushbutton pin  (DX)
  const int buttonPin2 = 4;     // the number of the pushbutton pin  (SX)

//=================================================[   Stato Bottoni   ]==========================================================

  int buttonState1 = 0;         // variable for reading the pushbutton status
  int buttonState2 = 0;         // variable for reading the pushbutton status
  
//--------------------------------------------------------------------------------------------------------------------------------

    const int maxX = 8;
    const int maxY = 8;

    int randX,  randY;


    unsigned char dispB[maxX];

    unsigned int board[maxY][maxX];

    int i = 4, j = 5, v = 4, m = 0, c = 0, s = 0;
    
    float t;
    
    int lunghezza;
    
    int h = 0;

    byte Cy[64];  // memoria della posizione Y della coda del serpente lungo tutto il gioco 
    byte Cx[64];  // memoria della posizione X della coda del serpente lungo tutto il gioco
 
//==================================================[    Funzione che permette di scrivere    ]===================================

void Write_Max7219_byte(unsigned char DATA) 
{   
    unsigned char i;
    for(i=1; i<=8;i++)
    {      
       digitalWrite(Max7219_pinCLK,LOW);
       digitalWrite(Max7219_pinDIN,DATA&0x80); // Estraggo il bit in 8 posizione (0x80 => 1000 0000)
       DATA = DATA<<1;                         // Shift a sinistra di un posto i bit in DATA
       digitalWrite(Max7219_pinCLK,HIGH);      // sull'arco di salita del CLK il DIN viene shiftato all'interno dello shift register
    }                                 
}

void Write_Max7219(unsigned char address,unsigned char dat)
{
    digitalWrite(Max7219_pinCS,LOW);  // Abilito la scrittura sul Data IN
    Write_Max7219_byte(address);      // indirizzo registro/comando da eseguire (bit D8 .. D11)
    Write_Max7219_byte(dat);          // bit da porre in D0..D7 (rappresentano il valore utilizzato dal comando) 
    digitalWrite(Max7219_pinCS,HIGH); // Termino la scrittura sul Data IN
}

//======================================================[    Funzione di inizializzazione Led 8x8  (Impostazioni)  ]==============

void Init_MAX7219(void)
{
   Write_Max7219(0x09, 0x00);       // decodifica: BCD
   Write_Max7219(0x0a, 0x01);       // luminosità  1..15
   Write_Max7219(0x0b, 0x07);       // limite scan；gestisco tutte le 8 righe
   Write_Max7219(0x0c, 0x01);       // Shutdown command: Display spento: 0x00 - Acceso (normal mode)：0x01
   Write_Max7219(0x0f, 0x00);       // Test command：Test Mode (tutti i LED accesi): 0x01 - Normal mode: 0x00)
}

//======================================================[      Funzione che passa dalla board al display i dati      ]============

void board2dispB( unsigned int b[][8], unsigned char d[] )
{
  int i, j;

    for( i = 0; i < maxX; i++ ) 
    {
      d[i] = 0;
      for( j = 0; j < maxY; j++ ) 
      {
        if( b[i][j] > 0 ) {
          d[i] += 1;
        }
        if( j < 7 )
          d[i] = d[i] << 1;
      }
    }
}

//=======================================================[        Writing Function         ]======================================

void writef()
  {
    board2dispB( board, dispB );
    for (int i=0;i<8;i++)
       Write_Max7219(i+1,dispB[i]);
 //      delay(50);
  }

//=======================================================[           Funzione di movimento           ]============================

int movementy( int y, int verso)
{
   if(verso == 2){
    y++;
    if(y == 8)
      y = 0;
    }
   if(verso == 4){
    y--;
    if(y == -1)
    y = 7;
   }

return y;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

int movementx(int x, int verso)
{
  if(verso == 1){
    x++;
    if(x == 8)
      x = 0;
   }
   if(verso == 3){
    x--;
    if(x == -1)
    x = 7;
   }
   return x;
}

//--------------------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------------------

//=====================================================[      Funzione di Setup      ]============================================

void setup()
{
   i = 4, // Posizione iniziale X
   j = 7, // Posizione iniziale Y
   v = 4, // Verso iniziale
   t = 1, // Numero del turno iniziale (1)
   m = 0, // Numero di mele mangiate dal serpente
   c = 0; // Indicatore della mela
   s = 0; // Indicatore di scontro
   lunghezza = 3; //Lunghezza iniziale del serpente
   
    // setto i pin come output
//--------------------------------------------------------------------------------------------------------------------------------
    pinMode(buttonPin1, INPUT);
    pinMode(buttonPin2, INPUT);
    pinMode(Max7219_pinCLK,OUTPUT);
    pinMode(Max7219_pinCS,OUTPUT);
    pinMode(Max7219_pinDIN,OUTPUT);
    delay(50);
    Init_MAX7219();

    // setup Board = 0
    for( int ix = 0; ix < 8; ix++ )
    for( int it = 0; it < 8; it++ )
    board[ix][it] = 0;

    writef();
    
    Serial.begin(9600);
    
    delay(2000);

}

//=======================================================[        Actual Program              ]===================================

void loop()
{
  if(s==0){
//  int i, j, v, t;

  buttonState1 = digitalRead(buttonPin1); // lettura stato primo pulsante
  buttonState2 = digitalRead(buttonPin2); // lettura stato secondo pulsante

//-----------------------------(1)       DECISIONE VERSO DA ESEGUIRE QUESTO TURNO-------------------------------------------------

//  if(buttonState1 == HIGH && buttonState2 == HIGH)
//  {

  if(buttonState1 == HIGH) {
    v++;
    if(v == 5)
      v = 1;
    }
  else{
    if(buttonState2 == HIGH) {
      v--;
      if(v == 0)
        v = 4;
      }
    }

//-----------------------------(1.2)     MELA-------------------------------------------------------------------------------------



  if(c == 0)
  {
    while(c == 0)
       {
        randX = random(maxX);
        randY = random(maxY);
        if(board[randY][randX]==0)
        {
         board[randY][randX] = 1;
        }
        if(board[randY][randX] = 1)
        c = 1;
       }
  }

  
//-----------------------------(2)       CALCOLO COORDINATE DA ACCENDERE QUESTO TURNO---------------------------------------------

  j = movementy(j, v);
  i = movementx(i, v);

//-----------------------------(3)       DEBUG------------------------------------------------------------------------------------

  Serial.print(Cx[0]);
  Serial.println(Cy[0]);
 
//-----------------------------(4)       CONDIZIONI DI SCONTRO CON SE STESSO------------------------------------------------------

  for(int conta = 0; conta < lunghezza-1; conta++)
    {
   if(j == Cy[conta])
     {
      if(i == Cx[conta])
      {
//        Write_Max7219(0x0a, 0x01);
//        Write_Max7219(0x0f, 0x01);
       s = 1;
      }
     }
    }

//-----------------------------(5)       CALCOLO DELLE COORDINATE DA SPEGNERE QUESTO TURNO----------------------------------------

if(s==0)
{
  board[Cy[0]][Cx[0]] = 0; //Spegnimento LED coda

  for(int conta = 0; conta < lunghezza-1; conta++)
  {
   Cx[conta] = Cx[conta+1];       //Shift a sinistra della coordinata X della coda
   Cy[conta] = Cy[conta+1];       //Shift a sinistra della coordinata Y della coda
  }
}
//-----------------------------(5.1)     AUMENTO LUNGHEZZA QUANDO IL SERPENTE MANGIA UNA MELA-------------------------------------

  if(j == randY && i == randX)
  {
   lunghezza++;
   m++;
   c = 0;
  }

//-----------------------------(6)       INFORMO LA BOARD DELLE COORDINATE DA ACCENDERE X LA TESTA--------------------------------


  board[j][i] = 1;         //  Accensione LED mela in caso il serpente ci sia sopra quando si crea
  board[randY][randX]=1;   //  Accensione LED testa del serpente
 
//-----------------------------(7)        EFFETTIVA ACCENSIONE E SPEGNIMENTO DEI LED 8X8------------------------------------------

  writef();

//-----------------------------(8)       STORAGE NELLA MEMORIA DELLE COORDINATE DELLA TESTA DI QUESTO TURNO ----------------------

                           //  Funziona come un timer, dopo un tot di turni uguale alla lunghezza del seprente quella che
                           //  prima era la testa diventa la coda perchè il serpente si muove e perciò viene cancellata
 
  Cx[lunghezza-1]= i;
  Cy[lunghezza-1]= j;


//-----------------------------(8.1)     Velocità del serpente (1 movimento ogni 400ms)-------------------------------------------
  if(s==0);
  delay(400);

//-----------------------------(9)       AUMENTO VARIABILI FINE DEL TURNO---------------------------------------------------------

  t++; //Turno del gioco
  }


//=========================== CASO DI FINE GIOCO =====================================
  if(s == 1){
    s = 0;
    for(int cont = 1; cont < 10; cont++)
    {
    Write_Max7219(0x0c, 0x00);
    delay(150);
    Write_Max7219(0x0c, 0x01);
    delay(150);
    }
        // setup Board = 0
    for( int ix = 0; ix < 8; ix++ )
    for( int it = 0; it < 8; it++ )
    board[ix][it] = 0;

    writef();
    
    delay(2000);
//----------------SCORE DEL GIOCO--------------------

 int contatore = 0;
  
  for(int ips = 7; ips >= 0; ips--)
  {
    for(int iks = 7; iks >= 0; iks--)
   {
    if(contatore < m)
    {
    board[ips][iks] = 1;
    writef();
    contatore++;
    delay(200);
    }
    else
    break;   
   }
  }
 
   delay(2000);
   
//--------------reinizizializzazione Gioco-----------

   i = 4, // Posizione iniziale X
   j = 7, // Posizione iniziale Y
   v = 4, // Verso iniziale
   t = 1, // Numero del turno iniziale (1)
   m = 0, // Numero di mele mangiate dal serpente
   c = 0; // Indicatore della mela
   s = 0; // Indicatore di scontro
   lunghezza = 3; //Lunghezza iniziale del serpente
   
    // setup Board = 0
    for( int ix = 0; ix < 8; ix++ )
    for( int it = 0; it < 8; it++ )
    board[ix][it] = 0;

    writef();
    
    Serial.begin(9600);
    
    delay(2000);

   }
}

