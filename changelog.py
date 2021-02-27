import pygit2

def collect_commits(repo):
	ret = []
	last = repo[repo.head.target]
	for commit in repo.walk(last.id, pygit2.GIT_SORT_TIME):
		shortmsg = commit.message.split('\n')[0]
		ret.append([shortmsg, commit.short_id])

	return ret

def get():
	repo = pygit2.Repository('.git')
	return collect_commits(repo)