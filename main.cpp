#include <SFML/Graphics.hpp>
#include <iostream>
#include <conio.h>
#include <math.h>

using namespace sf;

int size = 56;  // размер клетки

bool botMove = false; // первым ходит игрок
bool endOfGame = false;

Sprite figureSprite[18];

const int board[8][8] =
{
    1, 1, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 2, 2, 2,
	0, 0, 0, 0, 0, 2, 2, 2,
	0, 0, 0, 0, 0, 2, 2, 2
};


/////// AI ////////
int directionPriority[9][4];  // таблица приоритетных направлений ходов
									 // [i][0] - вверх
									 // [i][1] - вниз 
									 // [i][2] - вправо
									 // [i][3] - влево
								// i - номер игральной единицы ии

void prioritization();  // функция расстановки приоритетов

int* whichUnitMove();  // возвращает номер юнита и координаты хода

bool checkPosition(Vector2f pos, int dx, int dy);  // проверка вакантного хода на правильность
 
/////////---///////////

bool checkLines(Vector2f pos, int windowSize);
void setUnit();

int main()
{
	RenderWindow window(VideoMode(452, 452), "Corners");
	
	Texture figureTexture, boardTexture;
	figureTexture.loadFromFile("C://dev//SFML_Test//images//figures.png");
	boardTexture.loadFromFile("C://dev//SFML_Test//images//board0.png");

	Sprite boardSprite(boardTexture);
	
	for (int i = 0; i < 18; i++)
	{
		figureSprite[i].setTexture(figureTexture);
	}

	setUnit();
	prioritization();
	
	bool playerMove = false;
	
	int dx = 0, dy = 0;
	int n = 0;

	Vector2f oldPosition;

	
	
	while (window.isOpen())
	{
		Event e;

		Vector2i pos = Mouse::getPosition(window);

		while (window.pollEvent(e))
		{
			if (e.type == Event::Closed)
				window.close();
			

			if (e.type == Event::MouseButtonPressed)
			{
				if (e.key.code == Mouse::Left)
				{
					for (int i = 0; i < 9; i++) 
					{
						// если нажатие в область одного из спрайтов
						if (figureSprite[i].getGlobalBounds().contains(pos.x, pos.y))
						{
							playerMove = true;
							n = i;
							oldPosition = Vector2f(figureSprite[i].getPosition().x, figureSprite[i].getPosition().y);
							dx = pos.x - figureSprite[i].getPosition().x;
							dy = pos.y - figureSprite[i].getPosition().y;
							break;
						}
					}
					
				}
			}

			//---------------------------------------------------------------------------------------------------------

			if ((e.type == Event::MouseButtonReleased) && (playerMove))
			{
				if (e.key.code == Mouse::Left)
				{
					playerMove = false;
					Vector2f centre = figureSprite[n].getPosition() + Vector2f(size / 2, size / 2); 
					Vector2f newPosition = Vector2f(size*int(centre.x / size), size*int(centre.y / size));

					// если перемещение только в форме "+", не дальше одной клетки и не за границы игрового поля
					if ((((fabs(newPosition.x - oldPosition.x) == size) && (fabs(newPosition.y - oldPosition.y) == 0)) || 
						((fabs(newPosition.y - oldPosition.y) == size)&&(fabs(newPosition.x - oldPosition.x) == 0))) && 
						(checkLines(newPosition, window.getSize().x)))
					{
						for (int i = 0; i < 18; i++)
						{
							// если выбранная клетка занята другим спрайтом
							if ((newPosition.x == figureSprite[i].getPosition().x) && (newPosition.y == figureSprite[i].getPosition().y) && (n != i))
							{
								figureSprite[n].setPosition(oldPosition);
								botMove = false; // дать белым еще ход если ходили не правильно
								break;
							}
							else
							{
								figureSprite[n].setPosition(newPosition);
								if ((fabs(newPosition.x - oldPosition.x) == size) || (fabs(newPosition.y - oldPosition.y) == size))
									botMove = true; // ход темных
							}
						}

					}
					else
					{
						figureSprite[n].setPosition(oldPosition);
						botMove = false; // дать белым еще ход если ходили не правильно
					}

				}

			}

			//----------------------------------------------------------------------------------------------------------

		}

		// перемещение спрайта игрока
		if (playerMove)
		{
			if (n < 9) // если фишки игрока
			{
				figureSprite[n].setPosition(pos.x - dx, pos.y - dy);
			}
		}

		if (botMove)
		{
			int* pData = whichUnitMove();
			figureSprite[pData[0]].setPosition(figureSprite[pData[0]].getPosition().x + pData[1] * size,
											   figureSprite[pData[0]].getPosition().y + pData[2] * size);

			botMove = false;
		}
	
		//// Отрисовка ////

		window.clear(Color::White);
		window.draw(boardSprite);
		for (int i = 0; i < 18; i++)
		{
			window.draw(figureSprite[i]);
		}
		window.display();
	}
	return 0;
}

void setUnit() // установка фишек на игровое поле
{
	int k = 0;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (!board[i][j]) 
				continue;
		
			figureSprite[k].setTextureRect(IntRect(5 * size, (board[i][j] % 2)*56, size, size)); // светлые или темные
			figureSprite[k].setPosition(j*size, i*size);
			k++;  
		}
	}
}



void prioritization()  
{
	for (int i = 9; i < 18; i++)
	{
		if ((i == 10) || (i == 11) || (i == 14) || (i == 17))
		{
			directionPriority[i-9][0] = 40;  // высший приоритет
			directionPriority[i-9][3] = 30;
			directionPriority[i-9][1] = 20;
			directionPriority[i-9][2] = 10;  // низший приоритет
		}
		if ((i == 12) || (i == 15) || (i == 16) || (i == 9) || (i == 13))
		{
			directionPriority[i-9][3] = 40;
			directionPriority[i-9][0] = 30;
			directionPriority[i-9][2] = 20;
			directionPriority[i-9][1] = 10;
		}
		
	}
}

int* whichUnitMove()
{
	int maxPriority = INT16_MIN;
	int unit = -1;
	int dx = 0, dy = 0;
	int data[3];
	bool flag = false;
	for (int i = 9; i < 18; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (directionPriority[i-9][j] > maxPriority)
			{
				if ((j == 0) || (j == 1))
				{
					if (checkPosition(figureSprite[i].getPosition(), 0, -1 + 2*j )) // dx = 0, dy = 1 || -1
					{
						maxPriority = directionPriority[i-9][j];
						unit = i;
						dx = 0;
						dy = -1 + 2 * j;
					}
				}
				else if ((j == 2) || (j == 3))
				{
					if (checkPosition(figureSprite[i].getPosition(), 5 - 2*j,  0 )) // dy = 0, dx =  1 || -1
					{
						maxPriority = directionPriority[i-9][j];
						unit = i;
						dx = 5 - 2 * j;
						dy = 0;
					}
				}
			}
		}
	}
	switch (dx)  // пересмотри приоритета в зависимости от хода
	{
	case 0:
		directionPriority[unit - 9][3] += 10;
	case 1:
		directionPriority[unit - 9][2] -= 10;
	case -1:
		directionPriority[unit - 9][3] -= 10;
	}
	switch (dy)
	{
	case 0:
		directionPriority[unit - 9][0] += 10;
	case 1:
		directionPriority[unit - 9][1] -= 10;
	case -1:
		directionPriority[unit - 9][0] -= 10;
	}


	if (((figureSprite[unit].getPosition().x + dx * size) == 0 * size) && ((figureSprite[unit].getPosition().y + dy * size) == 0 * size))
	{
		for (int j = 0; j < 4; j++)
		{
			directionPriority[unit - 9][j] = INT16_MIN; // минимальный приоритет если фигура стоит там где нужно
		}
	}

	for (int i = 9; i < 18; i++)
	{
		if ((figureSprite[i].getPosition().x == 0) && (figureSprite[i].getPosition().y == 0))
		{
			flag = true;
			break;
		}
	}

	if (flag)
	{
		flag = false;
		if (((figureSprite[unit].getPosition().x + dx * size) == 56) && ((figureSprite[unit].getPosition().y + dy * size) == 0 * size))
		{
			for (int j = 0; j < 4; j++)
			{
				directionPriority[unit - 9][j] = INT16_MIN;
			}
			flag = true;
		}
		else flag = false;
		if (((figureSprite[unit].getPosition().x + dx * size) == 0) && ((figureSprite[unit].getPosition().y + dy * size) == 1 * size))
		{
			for (int j = 0; j < 4; j++)
			{
				directionPriority[unit - 9][j] = INT16_MIN;
			}
			flag = true;
		}
		else flag = false;
	}

	if (flag)
	{
		flag = false; 
		if (((figureSprite[unit].getPosition().x + dx * size) == 0 * size) && ((figureSprite[unit].getPosition().y + dy * size) == 2 * size))
		{
			for (int j = 0; j < 4; j++)
			{
				directionPriority[unit - 9][j] = INT16_MIN;
			}
			flag = true;
		}
		else flag = false;
		if (((figureSprite[unit].getPosition().x + dx * size) == 1 * size) && ((figureSprite[unit].getPosition().y + dy * size) == 1 * size))
		{
			for (int j = 0; j < 4; j++)
			{
				directionPriority[unit - 9][j] = INT16_MIN;
			}
			flag = true;
		}
		else flag = false;
	}

	if (flag)
	{
		if (((figureSprite[unit].getPosition().x + dx * size) == 1 * size) && ((figureSprite[unit].getPosition().y + dy * size) == 2 * size))
		{
			for (int j = 0; j < 4; j++)
			{
				directionPriority[unit - 9][j] = INT16_MIN;
			}
			flag = true;
		}
		else flag = false;
		if (((figureSprite[unit].getPosition().x + dx * size) == 2 * size) && ((figureSprite[unit].getPosition().y + dy * size) == 0 * size))
		{
			for (int j = 0; j < 4; j++)
			{
				directionPriority[unit - 9][j] = INT16_MIN;
			}
			flag = true;
		}
		else flag = false;
	}

	data[0] = unit; 
	data[1] = dx;
	data[2] = dy;

	return data;
}

bool checkPosition(Vector2f pos, int dx, int dy)  
{
	bool flag;
	for (int i = 0; i < 18; i++)
	{
		if (((pos.x + dx*size == figureSprite[i].getPosition().x) && (pos.y + dy*size == figureSprite[i].getPosition().y))
			|| (pos.x + dx*size < 0) || (pos.x + dx*size > size*7) || (pos.y + dy*size < 0) || (pos.y + dy*size > size*7))
		{
			flag = false;
			break;
		}
		else
		{
			flag = true;
		}
	}
	return flag;
}

bool checkLines(Vector2f pos, int windowSize)
{
	if ((pos.x < 0) || (pos.x > windowSize - 20) || (pos.y < 0) || (pos.y > windowSize - 20))
		return false;	
	else return true;
}

