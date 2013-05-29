model aa 
  parameter Integer N=10;
  Real a;
  Real b;
  Real k;
equation
  if time > 3 then
    a+k=b+456;
    b=345;
  else
    b=456;
    a+b=456;
  end if;
end a;
