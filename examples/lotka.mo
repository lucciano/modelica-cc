model lotka
  Real x1(start = 0.5),x2(start = 0.5);
equation
  der(x1) - 0.1 * x1 + 0.1 * x1 * x2 = 0;
  der(x2) - 0.1 * x1 * x2 + 0.1 * x2 = 0;
end lotka;

