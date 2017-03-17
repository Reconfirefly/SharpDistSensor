/*
SharpDistSensor.cpp
Source: https://github.com/DrGFreeman/SharpDistSensor

MIT License

Copyright (c) 2017 Julien de la Bruere-Terreault <drgfreeman@tuta.io>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <SharpDistSensor.h>

/* Constructor
  pin:    Arduino analog pin the sensor is connected to
  mfSize: Window size of the median filter
  valMin: Minimal analog value for which to return a distance
  valMax: Maximal analog value for which to return a distance
*/
SharpDistSensor::SharpDistSensor(const byte pin, const byte size) :
  medFilt(size, 1500)
{
  // Arduino analog pin the sensor is connected to
  _pin = pin;

  // Window size of the median filter (0 = no filtering)
  _mfSize = size;

  // Set default coefficients and range for Sharp GP2Y0A60SZLF 5V
  float coeffs[] = {1734, -9.005, 2.023E-2, -2.251E-5, 1.167E-8, -2.037E-12};
  setPolyFitCoeffs(6, coeffs, 30, 875);
}

// Return the measured distance
uint16_t SharpDistSensor::getDist()
{
  // Read analog value from sensor
  uint16_t sensVal = analogRead(_pin);

  // Constrain sensor values to remain within set min-max range
  if (sensVal < _valMin)
  {
    sensVal = _valMin;
  }
  else if (sensVal > _valMax)
  {
    sensVal = _valMax;
  }

  // Calculate distance from polynomial fit curve
  uint16_t dist = 0;
  dist += _coeffs[5] * pow(sensVal, 5);
  dist += _coeffs[4] * pow(sensVal, 4);
  dist += _coeffs[3] * pow(sensVal, 3);
  dist += _coeffs[2] * pow(sensVal, 2);
  dist += _coeffs[1] * sensVal;
  dist += _coeffs[0];

  if (_mfSize > 1)
  {
    // Get filtered distance value
    dist = medFilt.in(dist);
  }

  return dist;
}

// Set the polynomial fit curve coefficients and range
void SharpDistSensor::setPolyFitCoeffs(const byte nbCoeffs,
  const float* coeffs, const uint16_t valMin, const uint16_t valMax)
{
  // Set coefficients
  for (byte i = 0; i < 6; i++)
  {
    if (i < nbCoeffs)
    {
      // Coefficient is provided
      _coeffs[i] = coeffs[i];
    }
    else
    {
      // Coefficient is not provided, set to zero
      _coeffs[i] = 0;
    }
  }

  // Set analog value range
  setValMinMax(valMin, valMax);
}

// Set the analog value range for which to return a distance
void SharpDistSensor::setValMinMax(const uint16_t valMin, const uint16_t valMax)
{
    // Minimal analog value for which to return a distance
    _valMin = valMin;

    // Maximal analog value for which to return a distance
    _valMax = valMax;
}
