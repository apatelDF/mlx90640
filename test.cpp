#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"
#include <iostream>

using namespace std;

const byte MLX90640_address = 0x33; //Default 7-bit unshifted slave address of the MLX90640

#define TA_SHIFT 8 //Default shift for MLX90640 in open air

static float mlx90640To[768];
paramsMLX90640 mlx90640;

int main()
{
  // setup
  int status;
  uint16_t eeMLX90640[832];
  status = MLX90640_DumpEE(MLX90640_address, eeMLX90640);
  if (status != 0)
    cout<<"Failed to load system parameters\n";

  status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
  if (status != 0)
    cout<<"Parameter extraction failed\n";

  cout<<"Setup and Paramater extraction complete\n";

  // begin read data
  for (byte x = 0 ; x < 2 ; x++) //Read both subpages
  {
    uint16_t mlx90640Frame[834];
    int status = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
    if (status < 0)
    {
      cout<<"GetFrame Error: ";
      cout<<status;
      cout<<"\n";
    }

    float vdd = MLX90640_GetVdd(mlx90640Frame, &mlx90640);
    float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);

    cout<<"Ambient temperature: ";
    cout<<Ta;
    cout<<"\n";

    float tr = Ta - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
    float emissivity = 0.95;

    MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);
  }

  for (int x = 0 ; x < 10 ; x++)
  {
    cout<<"Pixel ";
    cout<<x;
    cout<<": ";
    cout<<mlx90640To[x];
    cout<<"C\n";
  }
}
