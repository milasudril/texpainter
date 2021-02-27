#!/usr/bin/python3

import pygit2
import datetime
import sys
import json

def collect_commits(repo):
	ret = []
	last = repo[repo.head.target]
	for commit in repo.walk(last.id, pygit2.GIT_SORT_TIME):
		shortmsg = commit.message.split('\n')[0]
		ret.append([commit.author.time, shortmsg, commit.short_id])

	return ret

def get():
	ret = dict()
	repo = pygit2.Repository('.git')
	ret['commit'] = repo.revparse_single('HEAD').hex
	ret['tag'] = repo.describe(show_commit_oid_as_fallback=True, dirty_suffix='-dirty')
	ret['changelog'] = collect_commits(repo)
	return ret

if __name__ == '__main__':
	data = get()
	with open('vcs_info.json', 'w') as f:
		json.dump(data, f)
