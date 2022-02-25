% Real time data collection example
%
% This script is implemented as a function so that it can
%   include sub-functions
%
%


%Send over bluetooth or serial
%serialPort = '/dev/tty.usbmodemM43210051'
%clear serialObject;

function plot_cams(serialObject)

% Instantiate variables
count = 1;
trace = zeros(1, 128); %Stored Values for Raw Input

while (1)
    % Check for data in the stream
    if serialObject.BytesAvailable
        val = fscanf(serialObject,'%i');
        if (val == -1) % -1 and -3 are start keywords
            count = 1;
        elseif (val == -2) % End camera1 tx
            if (count == 129)
                plot_cameras(trace);
            end
            count = 1;
        else
            if (count <= 128)
                trace(1,count) = val;
                count = count + 1;
            end
        end % if
    end %bytes available
end % while(1)

end

function [serialOut] = open_serial(path)
    serialOut = serial(path);
    serialOut.BaudRate = 115200;
    fopen(serialOut);
end

function close_serial(ser)
    fclose(serialObject);
    delete(serialObject);
    clear serialObject;
end

%*****************************************************************************************************************
%*****************************************************************************************************************

function plot_cameras(trace)

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
