import sys
import os
import json
import re
import requests
import argparse


def load_luogu_cookie():
    cookie = os.environ.get("LUOGU_COOKIE")
    if cookie:
        return cookie.strip()
    home_cookie = os.path.expanduser("~/.luogu_cookie")
    if os.path.exists(home_cookie):
        try:
            with open(home_cookie, "r", encoding="utf-8") as f:
                return f.read().strip()
        except (IOError, OSError, PermissionError) as e:
            print(f"Warning: Could not read cookie file: {e}", file=sys.stderr)
    return None


def solve_c3vk(session, html):
    match = re.search(r"C3VK=([a-f0-9]+)", html)
    if match:
        session.cookies.set("C3VK", match.group(1), domain="www.luogu.com.cn")
        return True
    return False


def get_with_retry(session, url, params=None):
    if params is None:
        params = {}
    try:
        r = session.get(url, params=params, timeout=10)
        if (
            r.text
            and "C3VK=" in r.text
            and not r.text.strip().startswith("{")
            and "lentille-context" not in r.text
        ):
            if solve_c3vk(session, r.text):
                r = session.get(url, params=params, timeout=10)
        return r
    except (requests.RequestException, requests.Timeout, requests.ConnectionError) as e:
        print(f"Warning: Request failed: {e}", file=sys.stderr)
        return None


def find_contest_for_problem(session, prob_id, uid):
    if uid:
        try:
            r = get_with_retry(
                session,
                f"https://www.luogu.com.cn/record/list",
                params={"user": uid, "_contentOnly": "1"},
            )
            if r is not None and r.status_code == 200:
                data = r.json()
                records = (
                    data.get("currentData", {}).get("records", {}).get("result", [])
                )
                for rec in records:
                    if rec.get("problem", {}).get("pid") == prob_id:
                        cid = rec.get("contest", {}).get("id")
                        if cid:
                            return cid
        except (json.JSONDecodeError, KeyError, TypeError) as e:
            print(f"Warning: Failed to parse record list: {e}", file=sys.stderr)
    try:
        r = get_with_retry(
            session,
            "https://www.luogu.com.cn/contest/list",
            params={"_contentOnly": "1"},
        )
        if r is not None and r.status_code == 200:
            data = r.json()
            contests = data.get("currentData", {}).get("contests", {}).get("result", [])
            for contest in contests[:30]:
                cid = contest.get("id")
                if not cid:
                    continue
                try:
                    cr = get_with_retry(
                        session,
                        f"https://www.luogu.com.cn/contest/{cid}",
                        params={"_contentOnly": "1"},
                    )
                    if cr is not None and cr.status_code == 200:
                        cdata = cr.json()
                        problems = cdata.get("currentData", {}).get(
                            "contestProblems", []
                        )
                        for p in problems:
                            if p.get("problem", {}).get("pid") == prob_id:
                                return cid
                except (json.JSONDecodeError, KeyError, TypeError):
                    continue
    except (json.JSONDecodeError, KeyError, TypeError) as e:
        print(f"Warning: Failed to parse contest list: {e}", file=sys.stderr)
    return None


def fetch_luogu_problem(prob_id, cookie=None, contest_id=None):
    session = requests.Session()
    session.headers.update(
        {"User-Agent": "Mozilla/5.0", "Referer": "https://www.luogu.com.cn/"}
    )
    uid = None
    if cookie:
        for item in cookie.split(";"):
            if "=" in item:
                k, v = item.strip().split("=", 1)
                session.cookies.set(k, v, domain="www.luogu.com.cn")
                if k == "_uid":
                    uid = v

    params = {"contestId": contest_id} if contest_id else {}
    res = get_with_retry(
        session, f"https://www.luogu.com.cn/problem/{prob_id}", params=params
    )

    if (
        res is not None
        and (
            res.status_code in (401, 403)
            or (res.status_code == 200 and "无权" in res.text)
        )
        and not contest_id
        and prob_id.startswith(("T", "U"))
    ):
        cid = find_contest_for_problem(session, prob_id, uid)
        if cid:
            res = get_with_retry(
                session,
                f"https://www.luogu.com.cn/problem/{prob_id}",
                params={"contestId": cid},
            )

    if not res:
        return None

    match = re.search(
        r'<script id="lentille-context" type="application/json">(.*?)</script>',
        res.text,
    )
    if match:
        try:
            data = json.loads(match.group(1))
            if data.get("template") == "error":
                return None
            return data.get("data", {}).get("problem")
        except (json.JSONDecodeError, KeyError, TypeError) as e:
            print(f"Warning: Failed to parse lentille-context: {e}", file=sys.stderr)

    try:
        data = res.json()
        if "currentData" in data:
            return data["currentData"].get("problem")
        if "data" in data and "problem" in data["data"]:
            return data["data"]["problem"]
    except (json.JSONDecodeError, ValueError) as e:
        print(f"Warning: Failed to parse JSON: {e}", file=sys.stderr)
    return None


def extract_code_blocks(content):
    """Extract all C++ code blocks from content, return list of (code, block_count)"""
    # Find all ```cpp or ```c++ blocks
    pattern = r"```(?:cpp|c\+\+)\s*(.*?)\s*```"
    matches = re.findall(pattern, content, re.DOTALL | re.IGNORECASE)

    code_blocks = []
    for match in matches:
        code = match.strip()
        if code and (
            "#include" in code
            or "int main" in code
            or "void" in code
            or "class" in code
        ):
            code_blocks.append(code)

    return code_blocks


def is_code_fragmented(code_blocks):
    """Check if code appears to be fragmented into too many small blocks"""
    if len(code_blocks) <= 1:
        return False
    # If there are many small blocks, it's likely fragmented
    if len(code_blocks) >= 4:
        return True
    # Check if blocks are very short (likely fragments)
    short_blocks = sum(1 for block in code_blocks if len(block) < 200)
    if short_blocks >= 2 and len(code_blocks) >= 2:
        return True
    return False


def merge_code_blocks(code_blocks):
    """Try to merge fragmented code blocks into a complete solution"""
    if not code_blocks:
        return None

    # Try to find the main solution (usually the largest block or the last one)
    # Strategy: if there's a block with 'int main', use that one
    for block in reversed(code_blocks):
        if "int main" in block or "signed main" in block:
            return block

    # Otherwise, return the last block (usually the complete solution)
    return code_blocks[-1]


def fetch_luogu_solutions(prob_id, cookie=None, contest_id=None):
    session = requests.Session()
    session.headers.update(
        {"User-Agent": "Mozilla/5.0", "Referer": "https://www.luogu.com.cn/"}
    )
    uid = None
    if cookie:
        for item in cookie.split(";"):
            if "=" in item:
                k, v = item.strip().split("=", 1)
                session.cookies.set(k, v, domain="www.luogu.com.cn")
                if k == "_uid":
                    uid = v

    params = {}
    if contest_id:
        params["contestId"] = contest_id
    res = get_with_retry(
        session, f"https://www.luogu.com.cn/problem/solution/{prob_id}", params=params
    )

    if (
        res is not None
        and (res.status_code in (401, 403))
        and not contest_id
        and prob_id.startswith(("T", "U"))
    ):
        cid = find_contest_for_problem(session, prob_id, uid)
        if cid:
            res = get_with_retry(
                session,
                f"https://www.luogu.com.cn/problem/solution/{prob_id}",
                params={"contestId": cid},
            )

    if res is None or res.status_code != 200:
        return None

    match = re.search(
        r'<script id="lentille-context" type="application/json">(.*?)</script>',
        res.text,
    )
    if match:
        try:
            data = json.loads(match.group(1))
            solutions = data.get("data", {}).get("solutions", {}).get("result", [])
        except (json.JSONDecodeError, KeyError, TypeError) as e:
            print(f"Warning: Failed to parse lentille-context: {e}", file=sys.stderr)
            return None
    else:
        try:
            data = res.json()
            solutions = (
                data.get("currentData", {}).get("solutions", {}).get("result", [])
            )
            if not solutions:
                solutions = data.get("data", {}).get("solutions", {}).get("result", [])
        except (json.JSONDecodeError, ValueError) as e:
            print(f"Warning: Failed to parse JSON response: {e}", file=sys.stderr)
            return None

    if not solutions:
        return None

    # Store first solution for fallback
    first_solution_code = None
    first_solution_blocks = []

    for idx, sol in enumerate(solutions):
        content = sol.get("content", "")
        code_blocks = extract_code_blocks(content)

        if not code_blocks:
            # Try fallback method for inline code
            if "#include" in content:
                lines = content.split("\n")
                code_lines = []
                started = False
                for line in lines:
                    if "#include" in line:
                        started = True
                    if started:
                        code_lines.append(line)
                if code_lines:
                    return "\n".join(code_lines)
            continue

        # Save first solution for later fallback
        if idx == 0:
            first_solution_blocks = code_blocks
            first_solution_code = merge_code_blocks(code_blocks)

        # Check if code is fragmented
        if is_code_fragmented(code_blocks):
            print(
                f"  Warning: Solution {idx + 1} appears to have fragmented code ({len(code_blocks)} blocks), trying next..."
            )
            continue

        # Return merged code from this solution
        code = merge_code_blocks(code_blocks)
        if code:
            print(f"  Using solution {idx + 1}")
            return code

    # If all solutions have fragmented code, use the first solution's last block
    if first_solution_code:
        print(
            f"  Warning: All solutions have fragmented code. Using last block from solution 1"
        )
        return first_solution_code

    return None


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("action")
    parser.add_argument("prob_id")
    parser.add_argument("--target_dir", default=".")
    parser.add_argument("--contest")
    parser.add_argument("--cookie")
    args = parser.parse_args()

    prob_id = args.prob_id.upper()
    cookie = args.cookie or load_luogu_cookie()

    if args.action == "samples":
        p_data = fetch_luogu_problem(prob_id, cookie, args.contest)
        if not p_data:
            print(f"Failed to fetch {prob_id}")
            sys.exit(1)
        samples = p_data.get("samples", [])
        tl = p_data.get("limits", {}).get("time", [1000])[0]
        data_dir = os.path.join(args.target_dir, "data")
        os.makedirs(data_dir, exist_ok=True)
        for i, s in enumerate(samples):
            if isinstance(s, list) and len(s) >= 2:
                inp, outp = s[0], s[1]
            elif isinstance(s, dict):
                inp, outp = s.get("input", ""), s.get("output", "")
            else:
                continue
            with open(os.path.join(data_dir, f"data{i + 1}.in"), "w") as f:
                f.write(inp.strip() + "\n")
            with open(os.path.join(data_dir, f"data{i + 1}.out"), "w") as f:
                f.write(outp.strip() + "\n")
        with open(os.path.join(args.target_dir, "info"), "w") as f:
            f.write(f"{len(samples)} {tl}\n")
        print(f"Successfully fetched samples for {prob_id}")

    elif args.action == "problem":
        p_data = fetch_luogu_problem(prob_id, cookie, args.contest)
        if not p_data:
            print(f"Failed to fetch {prob_id}")
            sys.exit(1)

        content = p_data.get("contenu", {})
        if not content:
            content = p_data

        with open(
            os.path.join(args.target_dir, "problem.md"), "w", encoding="utf-8"
        ) as f:
            f.write(f"# {p_data.get('title')}\n\n")
            if content.get("background"):
                f.write("## 题目背景\n\n" + content.get("background") + "\n\n")
            f.write("## 题目描述\n\n" + content.get("description", "") + "\n\n")
            # Correct keys are formatI and formatO
            f.write(
                "## 输入格式\n\n"
                + content.get("formatI", content.get("inputFormat", ""))
                + "\n\n"
            )
            f.write(
                "## 输出格式\n\n"
                + content.get("formatO", content.get("outputFormat", ""))
                + "\n\n"
            )

            samples = p_data.get("samples", [])
            for i, s in enumerate(samples):
                if isinstance(s, list) and len(s) >= 2:
                    inp, outp = s[0], s[1]
                elif isinstance(s, dict):
                    inp, outp = s.get("input", ""), s.get("output", "")
                else:
                    continue
                f.write(f"### 输入样例 {i + 1}\n\n```\n{inp.strip()}\n```\n\n")
                f.write(f"### 输出样例 {i + 1}\n\n```\n{outp.strip()}\n```\n\n")

            f.write("## 说明/提示\n\n" + content.get("hint", "") + "\n")
        print(f"Successfully saved problem.md for {prob_id}")

    elif args.action == "solve":
        code = fetch_luogu_solutions(prob_id, cookie, args.contest)
        if not code:
            print(f"Failed to fetch solution for {prob_id}")
            sys.exit(1)
        with open(os.path.join(args.target_dir, "std.cpp"), "w", encoding="utf-8") as f:
            f.write(code)
        print(f"Successfully saved std.cpp for {prob_id}")


if __name__ == "__main__":
    main()
