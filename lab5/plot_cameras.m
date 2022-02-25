% Real time data collection example
%
% This script is implemented as a function so that it can
%   include sub-functions
%
%

function plot_cams

%Send over bluetooth or serial
serialPort = '/dev/tty.KeySerial1';
serialObject = serial(serialPort);
%configure serial connection
serialObject.BaudRate = 115200; %(Default)
%serialObject.BaudRate = 115200;
%serialObject.FlowControl = 'software';

%Initiate serial connection
fopen(serialObject);

% This gets called on cleanup to ensure the stream gets closed
finishup = onCleanup(@() myCleanupFun(serialObject));

% Instantiate variables
count = 1;
trace = zeros(1, 128); %Stored Values for Raw Input
bintrace = zeros(1,128); %Stored Values for Edge Detection
smoothtrace  = zeros(1,128); %Stored Values for 5-Point Averager

while (1)
    % Check for data in the stream
    if serialObject.BytesAvailable
        val = fscanf(serialObject,'%i');
        %val
        if ((val == -1) || (val == -3)) % -1 and -3 are start keywords
            count = 1;
        elseif (val == -2) % End camera1 tx
            if (count == 128)
                plot_cameras(trace, 1);
            end %otherwise there was an error and don't plot
            count = 1;
            %plotdata(trace);
        elseif (val == -4) % End camera2 tx
            count = 1;
            plot_cameras(trace, 2);
        else
            trace(count) = val;
            count = count + 1;
        end % if
    end %bytes available
end % while(1)

%*****************************************************************************************************************
%*****************************************************************************************************************

function plot_cameras
trace = [2062 2127 2283 2358 2502 2574 2744 2817 3040 3048 3186 3228 3432 3424 3574 3625 3740 3796 3887 3887 4023 4036 4162 4177 4259 4193 4357 4331 4453 4385 4522 4489 4521 4547 4571 4544 4613 4456 4363 4157 4018 3796 3541 3299 3123 2867 2675 2417 2247 1959 1809 1586 1446 1250 1153 1068 1072 1048 1097 1038 1061 1038 1064 1054 1056 1046 1027 1018 1064 1011 1040 1009 1050 999 1039 990 1009 999 1023 1031 1032 1016 1028 1030 1048 1026 1066 1020 1045 1011 1020 1025 1035 1030 1041 1012 1023 1001 1014 970 984 951 959 915 917 905 904 869 866 827 855 823 847 781 807 773 778 752 773 725 727 697 718 697 695 673 670 670];

drawnow;
figure(1)
plot(trace);

%SMOOTH AND PLOT
smoothtrace = trace;
for i = 3:126
    smoothtrace(i) = (smoothtrace(i-2) + smoothtrace(i-1) + smoothtrace(i) ...
                     + smoothtrace(i+1) + smoothtrace(i+2)) / 5;
end

figure(2);
plot(smoothtrace);

%THRESHOLD
%calculate 1's and 0's via thresholding
threshold = 2000;
bintrace = smoothtrace;
for i = 1:128
    %Edge detection (binary 0 or 1)
    bintrace(i) = smoothtrace(i) > threshold;
end

figure(3)
plot(bintrace);

end