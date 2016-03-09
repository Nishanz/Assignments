%% Filtering the Audio Signal
% -By Ameya Prabhu

%Read input audio
% Roll No - 201402004 = 4 mod 8
[inp,fs_inp]=audioread('signal_4.wav');
figure;

%%Method Used - Remove high frequencies from fft world
% First remove the high frequencies for a smooth FFT
gaussFilter = gausswin(5);
gaussFilter = gaussFilter / sum(gaussFilter);
inp(:,1) = conv(inp(:,1), gaussFilter,'same');
inp(:,2) = conv(inp(:,2), gaussFilter,'same');

%Apply FFT on the filtered signal
fft_inp = fft(inp);
subplot(2,1,1);
plot(fft_inp);
inp_no = find(~(abs(fft_inp) > 40));
fft_inp(inp_no) = 0;

%Plot the output
subplot(2,1,2);
plot(fft_inp);
output = ifft(fft_inp);
audiowrite('output_q1.wav',output,fs_inp);