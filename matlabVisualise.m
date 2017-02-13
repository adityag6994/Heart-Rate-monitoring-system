%% Find local peaks and pulse segmentation
%clear all
close all
clc
for yoyo=1:10
cnt = 31;data_window_low = 1;data_window_hi = 1000;mafw =11;%load('5.mat');
%jojo=rand(1,1001);
xval=zeros();
for baby=1:1001
    xval(baby)=a.analogRead(2);
   % pause(0.001)
end
data =xval;

%% Smoothening
wts = [1/(2*(mafw-1));repmat(1/(mafw-1),(mafw-2),1);1/(2*(mafw-1))];
Ys = conv(data,wts,'same');
Ys = Ys(1,data_window_low:data_window_hi);
%% Finding peaks
loc_peak = 0;
window = 10;
for i = window/2+1 : length(Ys)-window/2
    temp = Ys(1,i-window/2:i+window/2);
    [x,y] = max(temp);
    %     if YB(1,i) == x
    if y == window/2+1
        loc_peak = [loc_peak i];
    end
end
loc_peak = loc_peak(1,2:end);
value_peak = Ys(1,loc_peak(1,:));

p_2_p = zeros(1,length(loc_peak)-1);
for i = 1 : length(loc_peak)-1
    p_2_p(1,i) = loc_peak(1,i+1) - loc_peak(1,i);
end

loc_false = zeros();
i = 1;
while  i <= length(p_2_p)
    if p_2_p(1,i) <= 0.6*mean(p_2_p)
        loc_false = [loc_false loc_peak(1,i+1)];
        i = i + 1;
    end
    i = i + 1;
end
loc_false = loc_false(1,2:end);
loc_peak = setdiff(loc_peak,loc_false);
value_peak = Ys(1,loc_peak(1,:));
 %% Finding minima
loc_min = 0;
window = 10;
for i = window/2+1 : length(Ys)-window/2
    temp = Ys(1,i-window/2:i+window/2);
    [x,y] = min(temp);
    %     if YB(1,i) == x
    if y == window/2+1
        loc_min = [loc_min i];
    end
end
loc_min = loc_min(1,2:end);
value_min = Ys(1,loc_min(1,:));

%% Segmenting pulses
loc_pulse_start=ones();
j = 1;
for i = 1 : length(loc_peak)
    while j <= length(loc_min) && loc_min(1,j) < loc_peak(1,i)
        if  value_peak(1,i) - value_min(1,j) >= 2
            loc_pulse_start(1,i) = loc_min(1,j);
        end
        j = j + 1;
    end
end
if(loc_pulse_start(1,1)) ==0
    loc_pulse_start = loc_pulse_start(1,2:end);
end
artif = 0;
pulse_flag = ones(1,length(loc_pulse_start-1));
for i = 1 : length(loc_pulse_start)
    if loc_pulse_start(1,i) == 0
        loc_pulse_start(1,i) = loc_pulse_start(1,i-1) + floor(mean(p_2_p));
        pulse_flag(1,i) = 0;
        artif = [artif loc_pulse_start(1,i) loc_pulse_start(1,i+1)];
    end
end
artif = artif(1,2:end);
value_pulse_start = zeros(1,length(loc_pulse_start));
for i = 1 : length(loc_pulse_start)
    value_pulse_start(1,i) = Ys(1,loc_pulse_start(1,i));
end
   
 
%%
xx = 1 : length(Ys);
figure
plot(xx,Ys,'k'), hold all
plot(xx(loc_pulse_start),value_pulse_start-0.05,'v','MarkerEdgeColor','k','markerfacecolor',[0 1 0]), hold all
plot(xx(loc_min),value_min-0.05,'v','MarkerEdgeColor','k','markerfacecolor',[0 1 0]), hold all
plot(xx(loc_peak),value_peak+0.05,'k^','markerfacecolor',[1 0 0]), hold all
%pause(.01)
end

