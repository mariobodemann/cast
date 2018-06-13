```
╱╲╱╱╱╱╲╱╲╲╲╲╲╱╲╲╱╲╱╱╲╲╲╲╱╱╲╱╲╱╲╲╲╲╲╱╲╲╱╱
╱╱╱╲╱╲╱╱╱╱╱╲╲╱╱╲╱╲╲╲╱╲╱╲╱╲╲╱╱╱╱╲╱╱╱╱╲╲╲╱
╲╲╲╲╲╱╱╲╲╱╱╲╲              ╱╲╲╱╱╲╱╱╱╲╱╱╱
╲╱╲╱╱╱╲╲╲╲╲╱╱  ASCII CAST  ╲╱╲╱╲╲╲╱╲╱╱╱╱
╱╲╲╱╲╱╱╱╱╱╲╲╱              ╱╱╲╱╲╱╱╱╲╲╲╱╱
╲╱╱╲╲╱╱╱╱╱╱╲╱╱╲╲╲╲╱╱╲╱╲╲╲╲╱╱╱╲╱╲╲╱╱╱╱╲╱╲
╲╱╱╲╱╲╲╱╲╲╱╱╲╲╲╱╱╱╲╱╲╲╱╱╱╱╲╱╲╲╲╱╱╲╱╱╲╲╱╱


┌──────────────────────────────────────┐
│                                      │
│                                      │
│                                      │
│                                      │
│                                      │
│WWW                                 WW│
│WWWWWWW                         WWWWWW│
│WWWWWWWWWWWWWWWWWWWWDDDDDDDDDWWWWWWWWW│
│WWWWWWWWWWWWWWWWWWWWDDDDDDDDDWWWWWWWWW│
│WWWWWWWWWWWWWWWWWWWWDDDDDDDDDWWWWWWWWW│
│WWWWWWWWWWWWWWWWWWWWDDDDDDDDDWWWWWWWWW│
│WWWWWWWWWWWWWWWWWWWWDDDDDDDDDWWWWWWWWW│
│WWWWWWW+++++++++++++++++++++++++WWWWWW│
│WWW+++++++++++++++++++++++++++++++++WW│
│++++++++++++++++++++++++++++++++++++++│
│++++++++++++++++++++++++++++++++++++++│
│++++++++++++++++++++++++++++++++++++++│
│++++++++++++++++++++++++++++++++++++++│
└──────────────────────────────────────┘

```
> cast w $(tput cols) h $(echo "$(tput lines) - 10" | bc) W $(echo $(tput cols) / 5 | bc)
