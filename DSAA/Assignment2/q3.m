%% Spectrogram making and plotting
%
% First - Load the files.
% Then, Calculate the stft by our Short-time Fourier Transform Function
% (Inspired by - Hristo Zhivomirov).
% After that, we can plot the appropriate plotting functions to obtain a
% Spectrogram.
% Finally, we compare it with the plot of the actual spectrogram as
% provided by Matlab.
% 
% Repeat this for laughter and train signals

load laughter; 
[S,F,T] = stft(y,256,128,Fs);
figure;
imagesc(F,T,20*log10(abs(S)))
axis xy; axis tight; colorbar; view(0,90);
ylabel('Time');
xlabel('Frequency (Hz)');
figure;
spectrogram(y,256,128,Fs);

load train;
[S,F,T] = stft(y,256,128,Fs);
figure;
imagesc(F,T,20*log10(abs(S)))
axis xy; axis tight; colorbar; view(0,90);
ylabel('Time');
xlabel('Frequency (Hz)');
figure;
spectrogram(y,256,128,Fs);