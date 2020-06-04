import pandas as pd
import sqlalchemy as sql
import mpld3 as d3
import matplotlib.pyplot as plt
import sys
field = sys.argv[1]
num = sys.argv[2]
opt = int(sys.argv[3])
db = sys.argv[4]
conn = sql.create_engine('mysql://dndd:dndddndd@localhost/' + db + '?charset=utf8')

rep = pd.read_sql("select * from Vote where num=" + num + " and field='" + field + "' and level>2 order by email, date desc", conn)
rep = rep.groupby('email').apply(lambda x : x.iloc[0, :])
reg = pd.read_sql("select * from Vote where num=" + num + " and field='" + field + "' and level<3 order by email, date desc", conn)
reg = reg.groupby('email').apply(lambda x : x.iloc[0, :])
follow = pd.read_sql('select * from Follow order by date desc', conn)
follow = follow.groupby('email').apply(lambda x: x.iloc[0,:])
auto = pd.merge(rep, follow, on='email')
if reg.size > 0: auto = auto[~auto['follower'].isin(reg['email']) ]
auto = auto.rename(columns = {'secret_y':'secret'})

option = [0] * opt
public = [0] * opt
label = [''] * opt
for i in range(opt): label[i] = str(i + 1)
for df in [rep, reg, auto]: 
    for index, row in df.iterrows(): 
        option[row['votedfor']-1] += 1
        if row['secret'] == 0: public[row['votedfor']-1] += 1
f, ar = plt.subplots(2,2)
ar[0,0].bar(range(opt),option)
ar[0,0].set_title('total')
ar[0,1].pie(option, labels=label, autopct='%1.1f%%')
ar[0,1].set_title('total')
ar[1,0].bar(range(opt),public)
ar[1,0].set_title('public')
ar[1,1].pie(public, labels=label, autopct='%1.1f%%')
ar[1,1].set_title('public')
f.subplots_adjust(hspace=0.3)
f.set_figheight(12)
f.set_figwidth(12)
ax = plt.gca()
fig = ax.get_figure()
print d3.fig_to_html(fig)

rep = rep[['votedfor']]
if reg.size: 
    reg = reg[reg['secret']==0]
    reg = reg[['votedfor']]
auto = auto[auto['secret']==0]
auto = auto[['email','follower', 'votedfor']]
print '<h2> Votings of Representatives</h2>'
print rep.to_html()
print '<h2> Votings of Regular by public</h2>'
print reg.to_html()
print '<h2> Votings of Followers by public</h2>'
print auto.to_html()
