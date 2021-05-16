#!/bin/bash

for N in {1..200}
do
    ruby Sending_Routers.rb $((($N%5)+1)) &
done

wait