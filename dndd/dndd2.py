import matplotlib
matplotlib.use('Agg')
import pandas as pd
import sqlalchemy as sql
import mpld3 as d3
import matplotlib.pyplot as plt
import sys
conn = sql.create_engine('mysql://dndd:dndddndd@localhost/dndd?charset=utf8')

field = sys.argv[1]
num = sys.argv[2]
opt = int(sys.argv[3])

rep = pd.read_sql("select * from Vote where num=" + num + " and field='" + field + "' and level>2 order by email, date desc", conn)
rep = rep.groupby('email').apply(lambda x : x.iloc[0, :])
reg = pd.read_sql("select * from Vote where num=" + num + " and field='" + field + "' and level<3 order by email, date desc", conn)
reg = reg.groupby('email').apply(lambda x : x.iloc[0, :])
follow = pd.read_sql('select * from Follow order by date desc', conn)
follow = follow.groupby('email').apply(lambda x: x.iloc[0,:])
auto = pd.merge(rep, follow, on='email')
if reg.size: auto = auto[~auto['follower'].isin(reg['email']) ]

option = [0] * opt
for df in [rep, reg, auto]: 
    for index, row in df.iterrows(): 
        option[row['votedfor']] += 1

#plt.bar(range(opt),option)
#plt.show()

plt.pie(option, labels=['1','2'])
ax = plt.gca()
fig = ax.get_figure()
print d3.fig_to_html(fig)
