R_lp = 20e3; % 20 kilo-Ohm
C_lp = 8e-9; % 8 nano-Farad
R_hp = 2e3;  % 2 kilo-Ohm
C_hp = 5e-6; % 5 micro-Farad

x = logspace(0, 5);
low_pass = 1.0 ./ (1 + 1i * (2.0 * pi * x * C_lp * R_lp));
low_pass_mag = 20 * log10(abs(low_pass));
low_pass_phase = angle(low_pass) * (180 ./ pi);

high_pass = 1i * (2.0 * pi * x * C_hp * R_hp) ./ (1 + 1i*(2.0 * pi * x * C_hp * R_hp));
high_pass_mag = 20 * log10(abs(high_pass));
high_pass_phase = angle(high_pass) * (180 ./ pi);

% Low pass code given the high_pass vector instead of x
band_pass = low_pass .* high_pass;
band_pass_mag = 20 * log10(abs(band_pass));
band_pass_phase = angle(band_pass) * (180 ./ pi);

figure
sgtitle("Low Pass filter");
ax = subplot(2,1,1);
ax.Position(2) = ax.Position(2) * 0.95;
semilogx(x, low_pass_mag);
hold on 
plot(994.7, -3, '.');
text(994.7, -3, 'F_H = 994.7 Hz');
title("Magnitude");
xlabel("Frequency (Hz)");
ylabel("Gain (dB)");
ax = subplot(2,1,2);
ax.Position(2) = ax.Position(2) * 0.95;
semilogx(x, low_pass_phase);
title("Phase");
xlabel("Frequency (Hz)");
ylabel("Angle (degrees)");
print('-dpng','-r0', "lpf");

figHandle = figure;
sgtitle("High Pass filter");
ax = subplot(2,1,1);
ax.Position(2) = ax.Position(2) * 0.95;
semilogx(x, high_pass_mag);
hold on 
plot(15.92, -3, '.');
text(15.92, -3, 'F_L = 15.92 Hz');
title("Magnitude");
xlabel("Frequency (Hz)");
ylabel("Gain (dB)");
ax = subplot(2,1,2);
ax.Position(2) = ax.Position(2) * 0.95;
semilogx(x, high_pass_phase);
title("Phase");
xlabel("Frequency (Hz)");
ylabel("Angle (degrees)");
print(figHandle,'-dpng','-r0', "hpf");

figHandle = figure;
sgtitle("Band Pass filter");
ax = subplot(2,1,1);
ax.Position(2) = ax.Position(2) * 0.95;
semilogx(x, band_pass_mag);
hold on 
plot(125.84, 0, '.');
text(125.84, -5, 'F_{center} = 125.84');
title("Magnitude");
xlabel("Frequency (Hz)");
ylabel("Gain (dB)");
ax = subplot(2,1,2);
ax.Position(2) = ax.Position(2) * 0.95;
semilogx(x, band_pass_phase);
title("Phase");
xlabel("Frequency (Hz)");
ylabel("Angle (degrees)");
print(figHandle,'-dpng','-r0', "bpf");
