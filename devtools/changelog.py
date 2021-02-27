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

def vcs_info():
	ret = dict()
	repo = pygit2.Repository('.git')
	ret['commit'] = repo.revparse_single('HEAD').hex
	ret['tag'] = repo.describe(show_commit_oid_as_fallback=True, dirty_suffix='-dirty')
	ret['changelog'] = collect_commits(repo)
	return ret

