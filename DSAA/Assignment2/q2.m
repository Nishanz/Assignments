%% Compression Ratios and Quality Estimation
% - By Ameya Prabhu
load('train');

a = [0.001,0.01,0.1,0.5];
quality = [1,2,3,4];

figure;
fft_y = fft(y);
subplot(3,2,2);
plot(y);
subplot(3,2,1);
plot(fft_y);

for i = 2:5
    T = a(i-1)*max(abs(fft_y));
    inp = size(fft_y);
    freq = find(fft_y < T);
    out = size(freq);
    fft_out = fft_y;
    fft_out(freq) = 0;
    y_out = ifft(fft_out);
    subplot(3,2,i);
    plot(y_out);
    disp(strcat('The compression ratio when the signal strength is ',num2str(a(i-1)) ,' is ',num2str((inp-out)/inp)));
    disp(strcat('I rate its quality in the order of most clear to least clear to be ',num2str(quality(i-1))));
end