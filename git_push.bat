set datestr=%date%
set result=%datestr:/=-%

git add .
git commit -am "Carregamento de backup (%result%)."
git push -f
