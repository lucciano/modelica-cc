/*DEPRECATED: modelicaOutput option no longer needed*/
class HalfWay_Rectifier
  Real R1__v(quantity = "ElectricPotential", unit = "V") "Voltage drop between the two pins (= p.v - n.v)";
  Real R1__i(quantity = "ElectricCurrent", unit = "A") "Current flowing from pin p to pin n";
  Real R1__p__v(quantity = "ElectricPotential", unit = "V") "Potential at the pin";
  Real R1__p__i(quantity = "ElectricCurrent", unit = "A") "Current flowing into the pin";
  Real R1__n__v(quantity = "ElectricPotential", unit = "V") "Potential at the pin";
  Real R1__n__i(quantity = "ElectricCurrent", unit = "A") "Current flowing into the pin";
  parameter Boolean R1__useHeatPort = false "=true, if HeatPort is enabled";
  parameter Real R1__T(quantity = "ThermodynamicTemperature", unit = "K", displayUnit = "degC", min = 0.0, start = 288.15, nominal = 300.0) = R1__T_ref "Fixed device temperature if useHeatPort = false";
  Real R1__LossPower(quantity = "Power", unit = "W") "Loss power leaving component via HeatPort";
  Real R1__T_heatPort(quantity = "ThermodynamicTemperature", unit = "K", displayUnit = "degC", min = 0.0, start = 288.15, nominal = 300.0) "Temperature of HeatPort";
  parameter Real R1__R(quantity = "Resistance", unit = "Ohm", start = 1.0) = 10.0 "Resistance at temperature T_ref";
  parameter Real R1__T_ref(quantity = "ThermodynamicTemperature", unit = "K", displayUnit = "degC", min = 0.0, start = 288.15, nominal = 300.0) = 300.15 "Reference temperature";
  parameter Real R1__alpha(quantity = "LinearTemperatureCoefficient", unit = "1/K") = 0.0 "Temperature coefficient of resistance (R_actual = R*(1 + alpha*(T_heatPort - T_ref))";
  Real R1__R_actual(quantity = "Resistance", unit = "Ohm") "Actual resistance = R*(1 + alpha*(T_heatPort - T_ref))";
  Real R2__v(quantity = "ElectricPotential", unit = "V") "Voltage drop between the two pins (= p.v - n.v)";
  Real R2__i(quantity = "ElectricCurrent", unit = "A") "Current flowing from pin p to pin n";
  Real R2__p__v(quantity = "ElectricPotential", unit = "V") "Potential at the pin";
  Real R2__p__i(quantity = "ElectricCurrent", unit = "A") "Current flowing into the pin";
  Real R2__n__v(quantity = "ElectricPotential", unit = "V") "Potential at the pin";
  Real R2__n__i(quantity = "ElectricCurrent", unit = "A") "Current flowing into the pin";
  parameter Boolean R2__useHeatPort = false "=true, if HeatPort is enabled";
  parameter Real R2__T(quantity = "ThermodynamicTemperature", unit = "K", displayUnit = "degC", min = 0.0, start = 288.15, nominal = 300.0) = R2__T_ref "Fixed device temperature if useHeatPort = false";
  Real R2__LossPower(quantity = "Power", unit = "W") "Loss power leaving component via HeatPort";
  Real R2__T_heatPort(quantity = "ThermodynamicTemperature", unit = "K", displayUnit = "degC", min = 0.0, start = 288.15, nominal = 300.0) "Temperature of HeatPort";
  parameter Real R2__R(quantity = "Resistance", unit = "Ohm", start = 1.0) = 50.0 "Resistance at temperature T_ref";
  parameter Real R2__T_ref(quantity = "ThermodynamicTemperature", unit = "K", displayUnit = "degC", min = 0.0, start = 288.15, nominal = 300.0) = 300.15 "Reference temperature";
  parameter Real R2__alpha(quantity = "LinearTemperatureCoefficient", unit = "1/K") = 0.0 "Temperature coefficient of resistance (R_actual = R*(1 + alpha*(T_heatPort - T_ref))";
  Real R2__R_actual(quantity = "Resistance", unit = "Ohm") "Actual resistance = R*(1 + alpha*(T_heatPort - T_ref))";
  Real ground__p__v(quantity = "ElectricPotential", unit = "V") "Potential at the pin";
  Real ground__p__i(quantity = "ElectricCurrent", unit = "A") "Current flowing into the pin";
  Real idealDiode__v(quantity = "ElectricPotential", unit = "V") "Voltage drop between the two pins (= p.v - n.v)";
  Real idealDiode__i(quantity = "ElectricCurrent", unit = "A") "Current flowing from pin p to pin n";
  Real idealDiode__p__v(quantity = "ElectricPotential", unit = "V") "Potential at the pin";
  Real idealDiode__p__i(quantity = "ElectricCurrent", unit = "A") "Current flowing into the pin";
  Real idealDiode__n__v(quantity = "ElectricPotential", unit = "V") "Potential at the pin";
  Real idealDiode__n__i(quantity = "ElectricCurrent", unit = "A") "Current flowing into the pin";
  parameter Boolean idealDiode__useHeatPort = false "=true, if HeatPort is enabled";
  parameter Real idealDiode__T(quantity = "ThermodynamicTemperature", unit = "K", displayUnit = "degC", min = 0.0, start = 288.15, nominal = 300.0) = 293.15 "Fixed device temperature if useHeatPort = false";
  Real idealDiode__LossPower(quantity = "Power", unit = "W") "Loss power leaving component via HeatPort";
  Real idealDiode__T_heatPort(quantity = "ThermodynamicTemperature", unit = "K", displayUnit = "degC", min = 0.0, start = 288.15, nominal = 300.0) "Temperature of HeatPort";
  parameter Real idealDiode__Ron(quantity = "Resistance", unit = "Ohm", min = 0.0) = 0.00001 "Forward state-on differential resistance (closed diode resistance)";
  parameter Real idealDiode__Goff(quantity = "Conductance", unit = "S", min = 0.0) = 0.00001 "Backward state-off conductance (opened diode conductance)";
  parameter Real idealDiode__Vknee(quantity = "ElectricPotential", unit = "V", min = 0.0, start = 0.0) "Forward threshold voltage";
  Boolean idealDiode__off(start = true) "Switching state";
  protected Real idealDiode__s(unit = "1", start = 0.0) "Auxiliary variable for actual position on the ideal diode characteristic";
  protected constant Real idealDiode__unitVoltage(quantity = "ElectricPotential", unit = "V") = 1.0;
  protected constant Real idealDiode__unitCurrent(quantity = "ElectricCurrent", unit = "A") = 1.0;
  Real C1__v(quantity = "ElectricPotential", unit = "V", start = 0.0) "Voltage drop between the two pins (= p.v - n.v)";
  Real C1__i(quantity = "ElectricCurrent", unit = "A") "Current flowing from pin p to pin n";
  Real C1__p__v(quantity = "ElectricPotential", unit = "V") "Potential at the pin";
  Real C1__p__i(quantity = "ElectricCurrent", unit = "A") "Current flowing into the pin";
  Real C1__n__v(quantity = "ElectricPotential", unit = "V") "Potential at the pin";
  Real C1__n__i(quantity = "ElectricCurrent", unit = "A") "Current flowing into the pin";
  parameter Real C1__C(quantity = "Capacitance", unit = "F", min = 0.0, start = 1.0) = 0.001 "Capacitance";
  Real sineVoltage__v(quantity = "ElectricPotential", unit = "V") "Voltage drop between the two pins (= p.v - n.v)";
  Real sineVoltage__i(quantity = "ElectricCurrent", unit = "A") "Current flowing from pin p to pin n";
  Real sineVoltage__p__v(quantity = "ElectricPotential", unit = "V") "Potential at the pin";
  Real sineVoltage__p__i(quantity = "ElectricCurrent", unit = "A") "Current flowing into the pin";
  Real sineVoltage__n__v(quantity = "ElectricPotential", unit = "V") "Potential at the pin";
  Real sineVoltage__n__i(quantity = "ElectricCurrent", unit = "A") "Current flowing into the pin";
  parameter Real sineVoltage__offset(quantity = "ElectricPotential", unit = "V") = 0.0 "Voltage offset";
  parameter Real sineVoltage__startTime(quantity = "Time", unit = "s") = 0.0 "Time offset";
  parameter Real sineVoltage__V(quantity = "ElectricPotential", unit = "V", start = 1.0) = 1.0 "Amplitude of sine wave";
  parameter Real sineVoltage__phase(quantity = "Angle", unit = "rad", displayUnit = "deg") = 0.0 "Phase of sine wave";
  parameter Real sineVoltage__freqHz(quantity = "Frequency", unit = "Hz", start = 1.0) = 50.0 "Frequency of sine wave";
  Real sineVoltage__signalSource__y "Connector of Real output signal";
  parameter Real sineVoltage__signalSource__amplitude = sineVoltage__V "Amplitude of sine wave";
  parameter Real sineVoltage__signalSource__freqHz(quantity = "Frequency", unit = "Hz", start = 1.0) = sineVoltage__freqHz "Frequency of sine wave";
  parameter Real sineVoltage__signalSource__phase(quantity = "Angle", unit = "rad", displayUnit = "deg") = sineVoltage__phase "Phase of sine wave";
  parameter Real sineVoltage__signalSource__offset = sineVoltage__offset "Offset of output signal";
  parameter Real sineVoltage__signalSource__startTime(quantity = "Time", unit = "s") = sineVoltage__startTime "Output = offset for time < startTime";
  protected constant Real sineVoltage__signalSource__pi = 3.141592653589793;
equation
  assert(1.0 + R1__alpha * (R1__T_heatPort - R1__T_ref) >= 0.000000000000001,"Temperature outside scope of model!");
  R1__R_actual = R1__R * (1.0 + R1__alpha * (R1__T_heatPort - R1__T_ref));
  R1__v = R1__R_actual * R1__i;
  R1__LossPower = R1__v * R1__i;
  R1__v = R1__p__v - R1__n__v;
  0.0 = R1__p__i + R1__n__i;
  R1__i = R1__p__i;
  R1__T_heatPort = R1__T;
  assert(1.0 + R2__alpha * (R2__T_heatPort - R2__T_ref) >= 0.000000000000001,"Temperature outside scope of model!");
  R2__R_actual = R2__R * (1.0 + R2__alpha * (R2__T_heatPort - R2__T_ref));
  R2__v = R2__R_actual * R2__i;
  R2__LossPower = R2__v * R2__i;
  R2__v = R2__p__v - R2__n__v;
  0.0 = R2__p__i + R2__n__i;
  R2__i = R2__p__i;
  R2__T_heatPort = R2__T;
  ground__p__v = 0.0;
  idealDiode__off = idealDiode__s < 0.0;
  idealDiode__v = idealDiode__s * (if idealDiode__off then 1.0 else idealDiode__Ron) + idealDiode__Vknee;
  idealDiode__i = idealDiode__s * (if idealDiode__off then idealDiode__Goff else 1.0) + idealDiode__Goff * idealDiode__Vknee;
  idealDiode__LossPower = idealDiode__v * idealDiode__i;
  idealDiode__v = idealDiode__p__v - idealDiode__n__v;
  0.0 = idealDiode__p__i + idealDiode__n__i;
  idealDiode__i = idealDiode__p__i;
  idealDiode__T_heatPort = idealDiode__T;
  C1__i = C1__C * der(C1__v);
  C1__v = C1__p__v - C1__n__v;
  0.0 = C1__p__i + C1__n__i;
  C1__i = C1__p__i;
  sineVoltage__signalSource__y = sineVoltage__signalSource__offset + (if time < sineVoltage__signalSource__startTime then 0.0 else sineVoltage__signalSource__amplitude * sin(6.283185307179586 * sineVoltage__signalSource__freqHz * (time - sineVoltage__signalSource__startTime) + sineVoltage__signalSource__phase));
  sineVoltage__v = sineVoltage__signalSource__y;
  sineVoltage__v = sineVoltage__p__v - sineVoltage__n__v;
  0.0 = sineVoltage__p__i + sineVoltage__n__i;
  sineVoltage__i = sineVoltage__p__i;
  R1__p__i + sineVoltage__p__i = 0.0;
  R1__n__i + idealDiode__p__i = 0.0;
  R2__p__i + idealDiode__n__i + C1__p__i = 0.0;
  R2__n__i + ground__p__i + C1__n__i + sineVoltage__n__i = 0.0;
  C1__n__v = R2__n__v;
  C1__n__v = ground__p__v;
  C1__n__v = sineVoltage__n__v;
  C1__p__v = R2__p__v;
  C1__p__v = idealDiode__n__v;
  R1__n__v = idealDiode__p__v;
  R1__p__v = sineVoltage__p__v;
end HalfWay_Rectifier;

