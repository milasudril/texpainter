import pygit2
import datetime


def collect_commits(repo):
	ret = []
	last = repo[repo.head.target]
	for commit in repo.walk(last.id, pygit2.GIT_SORT_TIME):
		shortmsg = commit.message.split('\n')[0]
		timestamp = datetime.datetime.fromtimestamp(commit.author.time)
		ret.append([str(timestamp), shortmsg, commit.short_id])

	return ret


def get():
	repo = pygit2.Repository('.git')
	return collect_commits(repo)
