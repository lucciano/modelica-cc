model RLC
  Real u2;
  Real u1;
  Real uL;
  Real iC;
  Real i2;
  Real u0;
  Real i0;
  Real i1;
  Real iL;
  Real uC;
  parameter Real R2;
  parameter Real L;
  parameter Real C;
equation
  u2 = R2 * i2;
  uL = L * der(iL);
  iC = C *der(uC);
  u0 = u1 + uC;
  uL = u1 + u2;
  uC = u2;
  u0 = i1 + iL;
  i1 = i2 + iC;
end RLC;
