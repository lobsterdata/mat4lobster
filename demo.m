%LOBSTER - Matlab Demo Code
%
%YOU MUST AGREE WITH THE TERMS SPECIFIED IN "license.txt" TO USE THE CODE.
%
%Introduction
% ----------------------------------------------------------
%
%   Welcome to the LOBSTER Matlab demo using lobster toolbox
%
%                          http://www.lobsterdata.com
% ----------------------------------------------------------
%
% The code provided below might help you get started with your LOBSTER
% data. The demo focuses on the two LOBSTER output files 'orderbook' and
% 'message'.
%
% You can find a detailed description of the LOBSTER data structure at
% http://LOBSTER.wiwi.hu-berlin.de
%
% Data used: AMZN - 2012-June-21 - 10 Levels
%
% $Author: Lobster team$ $Date: 2015/11/13$ $Version: 3.0$
% ----------------------------------------------------------

%% Set up the Basics
% Clear system

clear all;
clc;

% Stock name (NASDAQ symbol)

ticker      = 'AMZN';

% Number of levels of the order book
lvl         = 10;

% Date of the data

myday = '2012-06-21';

%% Load data
% file names of order book and message data files.

filenameBook = [ticker,'_', myday,'_34200000_57600000_orderbook_',int2str(lvl),'.csv'];
filenameMess = [ticker,'_', myday,'_34200000_57600000_message_',int2str(lvl),'.csv'];

% read in order book and message data

book = lobster.readOrderBook(filenameBook);
mess = lobster.readMessage(filenameMess);

%% Visualize order book
% convert prices into dollars

book(:,1:2:(4*lvl)) = book(:,1:2:(4*lvl))./10000;

% sample the order book at 10:30

idx = find(mess(:,1)<10.5*3600,1,'last');

% plot the order book.

figure(1)
lobster.plotOrderBook(book(idx,:));

% wait the user to hit return key.


%% Visualize trade information
%
% functions used: lobster.message2trade, lobster.getRegularIntervalIndices
%

% construct trade data. Limit orders executed less 0.1 second after the
% previous execution on the same side market are aggregated.

trades = lobster.message2trades(mess,0.1);

% Previous tick sampling for 5 minute interval from 9:30 to 16:00.

[idx,newTimeStamp]=lobster.getRegularIntervalIndices(trades(:,1), ...
    300,9.50*3600,16*3600);

% locate the memory. Note that LOBSTER data is typlically huge. For the
% sake of efficiency, locating memory before loading data is STONGLY
% recommended!

tradeInfo = zeros(length(newTimeStamp),3);

% trade price to dollar

tradeInfo(:,1) = trades(idx,2)/10000;

% number of trades for preceding 5 minutes. Note that the idx corresponding
% the number of trades before the time stamp. The difference of the idx
% hence is the number of trades taking place in last 5 minute interval.

tradeInfo(:,2) = [0;idx(2:end)-idx(1:end-1)];

% time stamp to seconds

newTimeStamp=newTimeStamp/3600;

% compute the trade volume in the previous time interval.

for i = 2:length(newTimeStamp)
    tradeInfo(i,3) = sum(trades(idx(i-1):idx(i),5));
end

% Visualization

figure(2)
% plot the trade price

subplot(3,1,1)
plot(newTimeStamp,tradeInfo(:,1));
xlim([newTimeStamp(1),newTimeStamp(end)])
ylabel('Price')

% plot the number of trades

subplot(3,1,2)
bar(newTimeStamp,tradeInfo(:,2))
xlim([newTimeStamp(1),newTimeStamp(end)])
ylabel('Number of trades')

% plot the trade volume

subplot(3,1,3)
bar(newTimeStamp,tradeInfo(:,3))
xlim([newTimeStamp(1),newTimeStamp(end)])
ylabel('Number of shares')


