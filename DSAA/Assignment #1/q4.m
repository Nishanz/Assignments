clear;clc;
tic;

%% Sound Simulation in 3 environments - Stairs, Tunnel, Club
%
% Read any input sound wave, threshold it to remove noise
% Then perform low-pass filtering so that noise is removed leading to
% better convolution, and then convolve it with the impulse response of the
% environment in GPU as time taken is significant, especially as file size
% increases.

[inp,fs_inp]=audioread('eminem.mp3');

%Select a channel
inp=inp(:,1);

%Scale the Input to keep it in range and threshold to remove noise
inp=inp*0.5;
inp(abs(inp)<0.03)=0;

% Apply Gaussian Filtering
gaussFilter = gausswin(7);
gaussFilter = gaussFilter / sum(gaussFilter);
inp = conv(inp, gaussFilter,'same');
audiowrite('eminem.wav',inp,fs_inp);
inp=gpuArray(inp);

%Repeat the same for all of the environment impulse responses too.
[stairs,fs_stairs]=audioread('stairs.wav');
stairs=stairs(:,1);
stairs(abs(stairs)<0.05)=0;
stairs=gpuArray(stairs);
outfile_stairs=gather(conv(inp,stairs,'same'));

[tunnel,fs_tunnel]=audioread('tunnel.wav');
tunnel=tunnel(:,1);
tunnel = tunnel*0.3;
tunnel(abs(tunnel)<0.05)=0;
tunnel=gpuArray(tunnel);
outfile_tunnel=gather(conv(inp,tunnel,'same'));

[club,fs_club]=audioread('club.wav');
club=club*0.25;
club=club(:,1);
club(abs(club)<0.07)=0;
club=gpuArray(club);
outfile_club=gather(conv(inp,club,'same'));

% Write the corresponding output audio files to PC

audiowrite('eminem_tunnel.wav',outfile_tunnel,fs_inp);
audiowrite('eminem_club.wav',outfile_club,fs_inp);
audiowrite('eminem_stairs.wav',outfile_stairs,fs_inp);

toc;